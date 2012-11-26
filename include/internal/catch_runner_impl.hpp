 /*
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RUNNER_IMPL_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_RUNNER_IMPL_HPP_INCLUDED

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
            LegacyReporterAdapter( m_reporter, ReporterConfig( m_config.stream(), m_config.data() ) ).testRunStarting( "" ); // !TBD - name
        }
        
        virtual ~Runner() {
            LegacyReporterAdapter( m_reporter, ReporterConfig( m_config.stream(), m_config.data() ) ).testRunEnding( TestRunStats( "", m_totals, aborting() ) ); // !TBD - name
            m_context.setRunner( m_prevRunner );
            m_context.setConfig( NULL );
            m_context.setResultCapture( m_prevResultCapture );
            m_context.setConfig( m_prevConfig );
        }

        Totals runMatching( const std::string& testSpec ) {

            std::vector<TestCase> matchingTests = getRegistryHub().getTestCaseRegistry().getMatchingTestCases( testSpec );

            Totals totals;

            LegacyReporterAdapter reporter( m_reporter, ReporterConfig( m_config.stream(), m_config.data() ) );
            
            reporter.testGroupStarting( testSpec );

            std::vector<TestCase>::const_iterator it = matchingTests.begin();
            std::vector<TestCase>::const_iterator itEnd = matchingTests.end();
            for(; it != itEnd; ++it )
                totals += runTest( *it );

            reporter.testGroupEnding( TestGroupStats( testSpec, totals, aborting() ) );
            return totals;
        }

        Totals runTest( const TestCase& testCase ) {
            Totals prevTotals = m_totals;

            std::string redirectedCout;
            std::string redirectedCerr;

            TestCaseInfo testInfo = testCase.getTestCaseInfo();

            LegacyReporterAdapter reporter( m_reporter, ReporterConfig( m_config.stream(), m_config.data() ) );
            reporter.testCaseStarting( testInfo );
            
            m_runningTest = new RunningTest( &testCase );

            do {
                do {
                    runCurrentTest( redirectedCout, redirectedCerr );
                }
                while( m_runningTest->hasUntestedSections() && !aborting() );
            }
            while( getCurrentContext().advanceGeneratorsForCurrentTest() && !aborting() );

            Totals deltaTotals = m_totals.delta( prevTotals );
            bool missingAssertions = false;
            if( deltaTotals.assertions.total() == 0  &&
               ( m_config.data().warnings & ConfigData::WarnAbout::NoAssertions ) ) {
                m_totals.assertions.failed++;
                deltaTotals = m_totals.delta( prevTotals );
                missingAssertions = true;
            }

            m_totals.testCases += deltaTotals.testCases;

            TestCaseStats stats( testInfo, deltaTotals, redirectedCout, redirectedCerr, missingAssertions, aborting() );
            reporter.testCaseEnding( stats );


            delete m_runningTest;
            m_runningTest = NULL;

            return deltaTotals;
        }

        const Config& config() const {
            return m_config;
        }
        
    private: // IResultCapture

        virtual ResultAction::Value acceptExpression( const ExpressionResultBuilder& assertionResult, const AssertionInfo& assertionInfo ) {
            m_lastAssertionInfo = assertionInfo;
            return actOnCurrentResult( assertionResult.buildResult( assertionInfo ) );
        }

        virtual void testEnded( const AssertionResult& result ) {
            LegacyReporterAdapter reporter( m_reporter, ReporterConfig( m_config.stream(), m_config.data() ) );
            if( result.getResultType() == ResultWas::Ok ) {
                m_totals.assertions.passed++;
            }
            else if( !result.isOk() ) {
                m_totals.assertions.failed++;

                {
                    std::vector<ScopedInfo*>::const_iterator it = m_scopedInfos.begin();
                    std::vector<ScopedInfo*>::const_iterator itEnd = m_scopedInfos.end();
                    for(; it != itEnd; ++it )
                        reporter.assertionEnding( AssertionStats( (*it)->buildResult( m_lastAssertionInfo ), m_totals ) );
                }
                {
                    std::vector<AssertionResult>::const_iterator it = m_assertionResults.begin();
                    std::vector<AssertionResult>::const_iterator itEnd = m_assertionResults.end();
                    for(; it != itEnd; ++it )
                        reporter.assertionEnding( AssertionStats( *it, m_totals ) );
                }
                m_assertionResults.clear();
            }
            
            if( result.getResultType() == ResultWas::Info )
            {
                m_assertionResults.push_back( result );
                m_totals.assertions.info++;
            }
            else
                reporter.assertionEnding( AssertionStats( result, m_totals ) );

            // Reset AssertionInfo
            m_lastAssertionInfo = AssertionInfo( "", m_lastAssertionInfo.lineInfo, "{Unknown expression after this line}" , m_lastAssertionInfo.resultDisposition );
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

            m_lastAssertionInfo.lineInfo = lineInfo;

            m_reporter->StartSection( name, description );
            assertions = m_totals.assertions;
            
            return true;
        }
        
        virtual void sectionEnded( const std::string& name, const Counts& prevAssertions ) {
            Counts assertions = m_totals.assertions - prevAssertions;
            if( assertions.total() == 0 &&
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
                ? m_runningTest->getTestCase().getTestCaseInfo().name
                : "";
        }

        virtual const AssertionResult* getLastResult() const {
            return &m_lastResult;            
        }

    public:
        // !TBD We need to do this another way!
        bool aborting() const {
            return m_totals.assertions.failed == static_cast<std::size_t>( m_config.getCutoff() );
        }

    private:

        ResultAction::Value actOnCurrentResult( const AssertionResult& result ) {
            m_lastResult = result;
            testEnded( m_lastResult );

            ResultAction::Value action = ResultAction::None;
            
            if( !m_lastResult.isOk() ) {
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
                m_lastAssertionInfo = AssertionInfo( "TEST_CASE", m_runningTest->getTestCase().getTestCaseInfo().lineInfo, "", ResultDisposition::Normal );
                m_runningTest->reset();
                if( m_reporter->shouldRedirectStdout() ) {
                    StreamRedirect coutRedir( std::cout, redirectedCout );
                    StreamRedirect cerrRedir( std::cerr, redirectedCerr );
                    m_runningTest->getTestCase().invoke();
                }
                else {
                    m_runningTest->getTestCase().invoke();
                }
                m_runningTest->ranToCompletion();
            }
            catch( TestFailureException& ) {
                // This just means the test was aborted due to failure
            }
            catch(...) {
                ExpressionResultBuilder exResult( ResultWas::ThrewException );
                exResult << translateActiveException();
                actOnCurrentResult( exResult.buildResult( m_lastAssertionInfo )  );
            }
            m_assertionResults.clear();
        }

    private:
        IMutableContext& m_context;
        RunningTest* m_runningTest;
        AssertionResult m_lastResult;

        const Config& m_config;
        Totals m_totals;
        Ptr<IReporter> m_reporter;
        std::vector<ScopedInfo*> m_scopedInfos;
        std::vector<AssertionResult> m_assertionResults;
        IRunner* m_prevRunner;
        IResultCapture* m_prevResultCapture;
        const IConfig* m_prevConfig;
        AssertionInfo m_lastAssertionInfo;
    };
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RUNNER_IMPL_HPP_INCLUDED
