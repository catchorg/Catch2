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
#include "catch_test_case_info.h"
#include "catch_capture.hpp"
#include "catch_totals.hpp"
#include "catch_test_spec.h"
#include "catch_test_case_tracker.hpp"
#include "catch_timer.h"

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
    class RunContext : public IResultCapture, public IRunner {

        RunContext( RunContext const& );
        void operator =( RunContext const& );

    public:

        explicit RunContext( Ptr<IConfig const> const& config, Ptr<IStreamingReporter> const& reporter )
        :   m_runInfo( config->name() ),
            m_context( getCurrentMutableContext() ),
            m_activeTestCase( NULL ),
            m_config( config ),
            m_reporter( reporter ),
            m_prevRunner( &m_context.getRunner() ),
            m_prevResultCapture( &m_context.getResultCapture() ),
            m_prevConfig( m_context.getConfig() )
        {
            m_context.setRunner( this );
            m_context.setConfig( m_config );
            m_context.setResultCapture( this );
            m_reporter->testRunStarting( m_runInfo );
        }

        virtual ~RunContext() {
            m_reporter->testRunEnded( TestRunStats( m_runInfo, m_totals, aborting() ) );
            m_context.setRunner( m_prevRunner );
            m_context.setConfig( NULL );
            m_context.setResultCapture( m_prevResultCapture );
            m_context.setConfig( m_prevConfig );
        }

        void testGroupStarting( std::string const& testSpec, std::size_t groupIndex, std::size_t groupsCount ) {
            m_reporter->testGroupStarting( GroupInfo( testSpec, groupIndex, groupsCount ) );
        }
        void testGroupEnded( std::string const& testSpec, Totals const& totals, std::size_t groupIndex, std::size_t groupsCount ) {
            m_reporter->testGroupEnded( TestGroupStats( GroupInfo( testSpec, groupIndex, groupsCount ), totals, aborting() ) );
        }

        Totals runMatching( std::string const& testSpec, std::size_t groupIndex, std::size_t groupsCount ) {

            std::vector<TestCase> matchingTests = getRegistryHub().getTestCaseRegistry().getMatchingTestCases( testSpec );

            Totals totals;

            testGroupStarting( testSpec, groupIndex, groupsCount );

            std::vector<TestCase>::const_iterator it = matchingTests.begin();
            std::vector<TestCase>::const_iterator itEnd = matchingTests.end();
            for(; it != itEnd; ++it )
                totals += runTest( *it );

            testGroupEnded( testSpec, totals, groupIndex, groupsCount );
            return totals;
        }

        Totals runTest( TestCase const& testCase ) {

            std::string redirectedCout;
            std::string redirectedCerr;

            TestCaseInfo testInfo = testCase.getTestCaseInfo();

            UnwindTestCaseOnCompletion finaliser(*this, m_totals, m_reporter, testInfo, redirectedCout, redirectedCerr);

            m_activeTestCase = &testCase;
            m_testCaseTracker = TestCaseTracker( testInfo.name );
            do {
                do {
                    runCurrentTest( redirectedCout, redirectedCerr );
                }
                while( !m_testCaseTracker->isCompleted() && !aborting() );
            }
            while( getCurrentContext().advanceGeneratorsForCurrentTest() && !aborting() );

            m_activeTestCase = NULL;
            m_testCaseTracker.reset();

            return finaliser.report();
        }

        Ptr<IConfig const> config() const {
            return m_config;
        }

    private: // IResultCapture

        virtual ResultAction::Value acceptExpression( ExpressionResultBuilder const& assertionResult, AssertionInfo const& assertionInfo ) {
            m_lastAssertionInfo = assertionInfo;
            return actOnCurrentResult( assertionResult.buildResult( assertionInfo ) );
        }

        virtual void assertionEnded( AssertionResult const& result ) {
            if( result.getResultType() == ResultWas::Ok ) {
                m_totals.assertions.passed++;
            }
            else if( !result.isOk() ) {
                m_totals.assertions.failed++;
            }

            if( m_reporter->assertionEnded( AssertionStats( result, m_messages, m_totals ) ) )
                m_messages.clear();

            // Reset working state
            m_lastAssertionInfo = AssertionInfo( "", m_lastAssertionInfo.lineInfo, "{Unknown expression after the reported line}" , m_lastAssertionInfo.resultDisposition );
        }

        virtual bool sectionStarted (
            SectionInfo const& sectionInfo,
            Counts& assertions
        )
        {
            std::ostringstream oss;
            oss << sectionInfo.name << "@" << sectionInfo.lineInfo;

            if( !m_testCaseTracker->enterSection( oss.str() ) )
                return false;

            m_lastAssertionInfo.lineInfo = sectionInfo.lineInfo;

            m_reporter->sectionStarting( sectionInfo );

            assertions = m_totals.assertions;

            return true;
        }
        bool testForMissingAssertions( Counts& assertions ) {
            if( assertions.total() != 0 ||
                    !m_config->warnAboutMissingAssertions() ||
                    m_testCaseTracker->currentSectionHasChildren() )
                return false;
            m_totals.assertions.failed++;
            assertions.failed++;
            return true;
        }

        void unwindSection(SectionInfo const& info, Counts const& prevAssertions, double _durationInSeconds ) {
            Counts assertions = m_totals.assertions - prevAssertions;
            bool missingAssertions = testForMissingAssertions( assertions );

            m_testCaseTracker->leaveSection();

            m_reporter->sectionEnded( SectionStats( info, assertions, _durationInSeconds, missingAssertions ) );
            m_messages.clear();
        }

        virtual void sectionEnded( SectionInfo const& info, Counts const& prevAssertions, double _durationInSeconds ) {
            if( std::uncaught_exception() ) {
                m_unfinishedSections.push_back( UnfinishedSections( info, prevAssertions, _durationInSeconds ) );
                return;
            }

            unwindSection(info, prevAssertions, _durationInSeconds);
        }

        virtual void pushScopedMessage( MessageInfo const& message ) {
            m_messages.push_back( message );
        }

        virtual void popScopedMessage( MessageInfo const& message ) {
            m_messages.erase( std::remove( m_messages.begin(), m_messages.end(), message ), m_messages.end() );
        }

        virtual bool shouldDebugBreak() const {
            return m_config->shouldDebugBreak();
        }

        virtual std::string getCurrentTestName() const {
            return m_activeTestCase
                ? m_activeTestCase->getTestCaseInfo().name
                : "";
        }

        virtual const AssertionResult* getLastResult() const {
            return &m_lastResult;
        }

    public:
        // !TBD We need to do this another way!
        bool aborting() const {
            return m_totals.assertions.failed == static_cast<std::size_t>( m_config->abortAfter() );
        }

    private:

        ResultAction::Value actOnCurrentResult( AssertionResult const& result ) {
            m_lastResult = result;
            assertionEnded( m_lastResult );

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
            TestCaseInfo const& testCaseInfo = m_activeTestCase->getTestCaseInfo();

            UnwindSectionOnCompletion finaliser(*this, m_totals, m_reporter, testCaseInfo, m_unfinishedSections, m_messages);
            try {
                m_lastAssertionInfo = AssertionInfo( "TEST_CASE", testCaseInfo.lineInfo, "", ResultDisposition::Normal );
                TestCaseTracker::Guard guard( *m_testCaseTracker );

                finaliser.startTimer();
                if( m_reporter->getPreferences().shouldRedirectStdOut ) {
                    StreamRedirect coutRedir( std::cout, redirectedCout );
                    StreamRedirect cerrRedir( std::cerr, redirectedCerr );
                    m_activeTestCase->invoke();
                }
                else {
                    m_activeTestCase->invoke();
                }
                finaliser.stopTimer();
            }
            catch( const Catch::TestFailureException& ) {
                // This just means the test was aborted due to failure
            }
            catch(...) {
                ExpressionResultBuilder exResult( ResultWas::ThrewException );
                exResult << translateActiveException();
                actOnCurrentResult( exResult.buildResult( m_lastAssertionInfo )  );
            }
        }

    private:
        struct UnfinishedSections {
            UnfinishedSections( SectionInfo const& _info, Counts const& _prevAssertions, double _durationInSeconds )
            : info( _info ), prevAssertions( _prevAssertions ), durationInSeconds( _durationInSeconds )
            {}

            SectionInfo info;
            Counts prevAssertions;
            double durationInSeconds;
        };

        class UnwindSectionOnCompletion
        {
        public:
            UnwindSectionOnCompletion(RunContext& context, Totals& totals, Ptr<IStreamingReporter>& reporter, TestCaseInfo const& testCaseInfo,
                std::vector<UnfinishedSections>& unfinishedSections, std::vector<MessageInfo>& messages)
                : m_context(context)
                , m_totals(totals)
                , m_reporter(reporter)
                , m_testCaseSection( testCaseInfo.name, testCaseInfo.description, testCaseInfo.lineInfo )
                , m_unfinishedSections(unfinishedSections)
                , m_messages(messages)
                , m_duration(0.0)
            {
                m_prevAssertions = m_totals.assertions;
                m_reporter->sectionStarting( m_testCaseSection );
            }
            ~UnwindSectionOnCompletion()
            {
                // If sections ended prematurely due to an exception we stored their
                // infos here so we can tear them down.
                for( std::vector<UnfinishedSections>::const_iterator it = m_unfinishedSections.begin(),
                            itEnd = m_unfinishedSections.end();
                        it != itEnd;
                        ++it ) {
                    m_context.unwindSection( it->info, it->prevAssertions, it->durationInSeconds );
                }
                m_unfinishedSections.clear();
                m_messages.clear();

                Counts assertions = m_totals.assertions - m_prevAssertions;
                bool missingAssertions = m_context.testForMissingAssertions( assertions );

                SectionStats testCaseSectionStats( m_testCaseSection, assertions, m_duration, missingAssertions );
                m_reporter->sectionEnded( testCaseSectionStats );
            }
            void startTimer()
            {
                m_timer.start();
            }
            void stopTimer()
            {
                m_duration = m_timer.getElapsedSeconds();
            }
        private:
            // non-copyable
            UnwindSectionOnCompletion(const UnwindSectionOnCompletion&);
            UnwindSectionOnCompletion& operator=(const UnwindSectionOnCompletion&);

            RunContext& m_context;
            Totals& m_totals;
            Ptr<IStreamingReporter>& m_reporter;
            SectionInfo m_testCaseSection;
            std::vector<UnfinishedSections>& m_unfinishedSections;
            std::vector<MessageInfo>& m_messages;
            Timer m_timer;
            Counts m_prevAssertions;
            double m_duration;
        };

        class UnwindTestCaseOnCompletion
        {
        public:
            UnwindTestCaseOnCompletion(RunContext& context, Totals& totals, Ptr<IStreamingReporter>& reporter, TestCaseInfo& testInfo,
                std::string& redirectedCout, std::string& redirectedCerr)
                : m_context(context), m_totals(totals), m_reporter(reporter), m_testInfo(testInfo)
                , m_redirectedCout(redirectedCout), m_redirectedCerr(redirectedCerr)
                , m_reported(false)
            {
                m_prevTotals = m_totals;
                m_reporter->testCaseStarting( m_testInfo );
            }
            ~UnwindTestCaseOnCompletion()
            {
                if( !m_reported )
                {
                    report();
                }
            }
            Totals report()
            {
                m_reported = true;
                Totals deltaTotals = m_totals.delta( m_prevTotals );
                m_totals.testCases += deltaTotals.testCases;
                m_reporter->testCaseEnded( TestCaseStats(   m_testInfo,
                                                            deltaTotals,
                                                            m_redirectedCout,
                                                            m_redirectedCerr,
                                                            m_context.aborting() ) );
                return deltaTotals;
            }
        private:
            // non-copyable
            UnwindTestCaseOnCompletion(const UnwindTestCaseOnCompletion&);
            UnwindTestCaseOnCompletion& operator=(const UnwindTestCaseOnCompletion&);

            RunContext& m_context;
            Totals& m_totals;
            Ptr<IStreamingReporter>& m_reporter;
            TestCaseInfo& m_testInfo;
            std::string& m_redirectedCout;
            std::string& m_redirectedCerr;
            bool m_reported;
            Totals m_prevTotals;
        };

        TestRunInfo m_runInfo;
        IMutableContext& m_context;
        TestCase const* m_activeTestCase;
        Option<TestCaseTracker> m_testCaseTracker;
        AssertionResult m_lastResult;

        Ptr<IConfig const> m_config;
        Totals m_totals;
        Ptr<IStreamingReporter> m_reporter;
        std::vector<MessageInfo> m_messages;
        IRunner* m_prevRunner;
        IResultCapture* m_prevResultCapture;
        Ptr<IConfig const> m_prevConfig;
        AssertionInfo m_lastAssertionInfo;
        std::vector<UnfinishedSections> m_unfinishedSections;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RUNNER_IMPL_HPP_INCLUDED
