 /*
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_INTERNAL_CATCH_RUNNER_HPP_INCLUDED
#define TWOBLUECUBES_INTERNAL_CATCH_RUNNER_HPP_INCLUDED

#include "catch_interfaces_runner.h"
#include "catch_interfaces_reporter.h"
#include "catch_interfaces_exception.h"
#include "catch_config.hpp"
#include "catch_test_registry.hpp"
#include "catch_test_case_info.h"
#include "catch_capture.hpp"
#include "catch_totals.hpp"
#include "catch_running_test.hpp"
#include "catch_test_spec.h"

#include <set>
#include <string>

namespace Catch {

    class StreamRedirect {
    
    public:
        StreamRedirect( std::ostream& stream, std::string& targetString )
        :   m_stream( stream ),
            m_prevBuf( stream.rdbuf() ),
            m_targetString( targetString ) 
        {            
            stream.rdbuf( m_oss.rdbuf() );
        }
        
        ~StreamRedirect() {
            m_targetString += m_oss.str();
            m_stream.rdbuf( m_prevBuf );
        }
        
    private:
        std::ostream& m_stream;
        std::streambuf* m_prevBuf;
        std::ostringstream m_oss;
        std::string& m_targetString;
    };
    
    ///////////////////////////////////////////////////////////////////////////

    class Runner : public IResultCapture, public IRunner {
    
        Runner( const Runner& );
        void operator =( const Runner& );
        
    public:

        explicit Runner( const Config& config, const Ptr<IReporter>& reporter )
        :   m_context( getCurrentMutableContext() ),
            m_runningTest( NULL ),
            m_config( config ),
            m_reporter( reporter ),
            m_prevRunner( &m_context.getRunner() ),
            m_prevResultCapture( &m_context.getResultCapture() ),
            m_prevConfig( m_context.getConfig() )
        {
            m_context.setRunner( this );
            m_context.setConfig( &m_config );
            m_context.setResultCapture( this );
            m_reporter->StartTesting();
        }
        
        virtual ~Runner() {
            m_reporter->EndTesting( m_totals );
            m_context.setRunner( m_prevRunner );
            m_context.setConfig( NULL );
            m_context.setResultCapture( m_prevResultCapture );
            m_context.setConfig( m_prevConfig );
        }

        Totals runMatching( const std::string& testSpec ) {

            std::vector<TestCaseInfo> matchingTests = getRegistryHub().getTestCaseRegistry().getMatchingTestCases( testSpec );

            Totals totals;

            m_reporter->StartGroup( testSpec );

            std::vector<TestCaseInfo>::const_iterator it = matchingTests.begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = matchingTests.end();
            for(; it != itEnd; ++it )
                totals += runTest( *it );
            // !TBD use std::accumulate?

            m_reporter->EndGroup( testSpec, totals );
            return totals;
        }

        Totals runTest( const TestCaseInfo& testInfo ) {
            Totals prevTotals = m_totals;

            std::string redirectedCout;
            std::string redirectedCerr;
            
            m_reporter->StartTestCase( testInfo );
            
            m_runningTest = new RunningTest( &testInfo );

            do {
                do {
                    m_currentResult.setLineInfo( m_runningTest->getTestCaseInfo().getLineInfo() );
                    runCurrentTest( redirectedCout, redirectedCerr );
                }
                while( m_runningTest->hasUntestedSections() && !aborting() );
            }
            while( getCurrentContext().advanceGeneratorsForCurrentTest() && !aborting() );

            delete m_runningTest;
            m_runningTest = NULL;

            Totals deltaTotals = m_totals.delta( prevTotals );
            m_totals.testCases += deltaTotals.testCases;            
            m_reporter->EndTestCase( testInfo, deltaTotals, redirectedCout, redirectedCerr );
            return deltaTotals;
        }

        const Config& config() const {
            return m_config;
        }
        
    private: // IResultCapture

        virtual ResultAction::Value acceptResult( bool result ) {
            return acceptResult( result ? ResultWas::Ok : ResultWas::ExpressionFailed );
        }

        virtual ResultAction::Value acceptResult( ResultWas::OfType result ) {
            m_currentResult.setResultType( result );            
            return actOnCurrentResult();
        }

        virtual ResultAction::Value acceptExpression( const ResultInfoBuilder& resultInfo ) {
            m_currentResult = resultInfo;
            return actOnCurrentResult();
        }

        virtual void acceptMessage( const std::string& msg ) {
            m_currentResult.setMessage( msg );
        }
                
        virtual void testEnded( const ResultInfo& result ) {
            if( result.getResultType() == ResultWas::Ok ) {
                m_totals.assertions.passed++;
            }
            else if( !result.ok() ) {
                m_totals.assertions.failed++;

                std::vector<ResultInfo>::const_iterator it = m_info.begin();
                std::vector<ResultInfo>::const_iterator itEnd = m_info.end();
                for(; it != itEnd; ++it )
                    m_reporter->Result( *it );
                m_info.clear();
            }
            
            if( result.getResultType() == ResultWas::Info )
                m_info.push_back( result );
            else
                m_reporter->Result( result );
        }
        
        virtual bool sectionStarted (
            const std::string& name, 
            const std::string& description,
            const SourceLineInfo& lineInfo,
            Counts& assertions
        )
        {
            std::ostringstream oss;
            oss << name << "@" << lineInfo;

            if( !m_runningTest->addSection( oss.str() ) )
                return false;

            m_currentResult.setLineInfo( lineInfo );
            m_reporter->StartSection( name, description );
            assertions = m_totals.assertions;
            
            return true;
        }
        
        virtual void sectionEnded( const std::string& name, const Counts& prevAssertions ) {
            Counts assertions = m_totals.assertions - prevAssertions;
            if( assertions.total() == 0  &&
               ( m_config.data().warnings & ConfigData::WarnAbout::NoAssertions ) &&
               !m_runningTest->isBranchSection() ) {
                m_reporter->NoAssertionsInSection( name );
                m_totals.assertions.failed++;
                assertions.failed++;
            }
            m_runningTest->endSection( name );
            m_reporter->EndSection( name, assertions );
        }

        virtual void pushScopedInfo( ScopedInfo* scopedInfo ) {
            m_scopedInfos.push_back( scopedInfo );
        }

        virtual void popScopedInfo( ScopedInfo* scopedInfo ) {
            if( m_scopedInfos.back() == scopedInfo )
                m_scopedInfos.pop_back();
        }

        virtual bool shouldDebugBreak() const {        
            return m_config.shouldDebugBreak();
        }

        virtual std::string getCurrentTestName() const {
            return m_runningTest
                ? m_runningTest->getTestCaseInfo().getName()
                : "";
        }

        virtual const ResultInfo* getLastResult() const {
            return &m_lastResult;            
        }

    public:
        // !TBD We need to do this another way!
        bool aborting() const {
            return m_totals.assertions.failed == static_cast<std::size_t>( m_config.getCutoff() );
        }

    private:

        ResultAction::Value actOnCurrentResult() {
            testEnded( m_currentResult );
            m_lastResult = m_currentResult;
            
            m_currentResult = ResultInfoBuilder();

            ResultAction::Value action = ResultAction::None;
            
            if( !m_lastResult.ok() ) {
                action = ResultAction::Failed;
                if( shouldDebugBreak() )
                    action = (ResultAction::Value)( action | ResultAction::Debug );
                if( aborting() )
                    action = (ResultAction::Value)( action | ResultAction::Abort );
            }
            return action;
        }

        void runCurrentTest( std::string& redirectedCout, std::string& redirectedCerr ) {
            try {
                m_runningTest->reset();
                Counts prevAssertions = m_totals.assertions;
                if( m_reporter->shouldRedirectStdout() ) {
                    StreamRedirect coutRedir( std::cout, redirectedCout );
                    StreamRedirect cerrRedir( std::cerr, redirectedCerr );
                    m_runningTest->getTestCaseInfo().invoke();
                }
                else {
                    m_runningTest->getTestCaseInfo().invoke();
                }
                Counts assertions = m_totals.assertions - prevAssertions;
                if( assertions.total() == 0  &&
                   ( m_config.data().warnings & ConfigData::WarnAbout::NoAssertions ) &&
                   !m_runningTest->hasSections() ) {
                        m_totals.assertions.failed++;                        
                        m_reporter->NoAssertionsInTestCase( m_runningTest->getTestCaseInfo().getName() );
                }
                m_runningTest->ranToCompletion();
            }
            catch( TestFailureException& ) {
                // This just means the test was aborted due to failure
            }
            catch(...) {
                acceptMessage( getRegistryHub().getExceptionTranslatorRegistry().translateActiveException() );
                acceptResult( ResultWas::ThrewException );
            }
            m_info.clear();
        }

    private:
        IMutableContext& m_context;
        RunningTest* m_runningTest;
        ResultInfoBuilder m_currentResult;
        ResultInfo m_lastResult;

        const Config& m_config;
        Totals m_totals;
        Ptr<IReporter> m_reporter;
        std::vector<ScopedInfo*> m_scopedInfos;
        std::vector<ResultInfo> m_info;
        IRunner* m_prevRunner;
        IResultCapture* m_prevResultCapture;
        const IConfig* m_prevConfig;
    };
    
} // end namespace Catch

#endif // TWOBLUECUBES_INTERNAL_CATCH_RUNNER_HPP_INCLUDED
