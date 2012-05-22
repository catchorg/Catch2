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
#include "catch_test_case_info.hpp"
#include "catch_capture.hpp"
#include "catch_totals.hpp"
#include "catch_running_test.hpp"

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

        explicit Runner( Config& config )
        :   m_context( getCurrentMutableContext() ),
            m_runningTest( NULL ),
            m_config( config ),
            m_reporter( config.getReporter() ),
            m_prevRunner( &m_context.getRunner() ),
            m_prevResultCapture( &m_context.getResultCapture() )
        {
            m_context.setRunner( this );
            m_context.setResultCapture( this );
            m_reporter->StartTesting();
        }
        
        ~Runner() {
            m_reporter->EndTesting( m_totals );
            m_context.setRunner( m_prevRunner );
            m_context.setResultCapture( m_prevResultCapture );
        }
        
        virtual void runAll( bool runHiddenTests = false ) {
            const std::vector<TestCaseInfo>& allTests = getCurrentContext().getTestCaseRegistry().getAllTests();
            for( std::size_t i=0; i < allTests.size(); ++i ) {
                if( runHiddenTests || !allTests[i].isHidden() )
                   runTest( allTests[i] );
            }
        }
        
        virtual std::size_t runMatching( const std::string& rawTestSpec ) {
            TestSpec testSpec( rawTestSpec );
            
            const std::vector<TestCaseInfo>& allTests = getCurrentContext().getTestCaseRegistry().getAllTests();
            std::size_t testsRun = 0;
            for( std::size_t i=0; i < allTests.size(); ++i ) {
                if( testSpec.matches( allTests[i].getName() ) ) {
                    runTest( allTests[i] );
                    testsRun++;
                }
            }
            return testsRun;
        }
        
        void runTest( const TestCaseInfo& testInfo ) {
            Totals prevTotals = m_totals;

            std::string redirectedCout;
            std::string redirectedCerr;
            
            m_reporter->StartTestCase( testInfo );
            
            m_runningTest = new RunningTest( &testInfo );

            do {
                do {
                    m_reporter->StartGroup( "test case run" );
                    m_currentResult.setLineInfo( m_runningTest->getTestCaseInfo().getLineInfo() );
                    runCurrentTest( redirectedCout, redirectedCerr );
                    m_reporter->EndGroup( "test case run", m_totals.delta( prevTotals ) );
                }
                while( m_runningTest->hasUntestedSections() );
            }
            while( getCurrentContext().advanceGeneratorsForCurrentTest() );

            delete m_runningTest;
            m_runningTest = NULL;

            Totals deltaTotals = m_totals.delta( prevTotals );
            m_totals.testCases += deltaTotals.testCases;            
            m_reporter->EndTestCase( testInfo, deltaTotals, redirectedCout, redirectedCerr );
        }

        virtual Totals getTotals() const {
            return m_totals;
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
            m_runningTest->endSection( name );
            m_reporter->EndSection( name, m_totals.assertions - prevAssertions );
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
        
    private:
        
        ResultAction::Value actOnCurrentResult() {
            testEnded( m_currentResult );
            m_lastResult = m_currentResult;
            
            m_currentResult = ResultInfoBuilder();
            if( m_lastResult.ok() )
                return ResultAction::None;
            else if( shouldDebugBreak() )
                return ResultAction::DebugFailed;
            else
                return ResultAction::Failed;
        }

        void runCurrentTest( std::string& redirectedCout, std::string& redirectedCerr ) {
            try {
                m_runningTest->reset();
                if( m_reporter->shouldRedirectStdout() ) {
                    StreamRedirect coutRedir( std::cout, redirectedCout );
                    StreamRedirect cerrRedir( std::cerr, redirectedCerr );
                    m_runningTest->getTestCaseInfo().invoke();
                }
                else {
                    m_runningTest->getTestCaseInfo().invoke();
                }
                m_runningTest->ranToCompletion();
            }
            catch( TestFailureException& ) {
                // This just means the test was aborted due to failure
            }
            catch(...) {
                acceptMessage( getCurrentContext().getExceptionTranslatorRegistry().translateActiveException() );
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
    };
    
} // end namespace Catch

#endif // TWOBLUECUBES_INTERNAL_CATCH_RUNNER_HPP_INCLUDED
