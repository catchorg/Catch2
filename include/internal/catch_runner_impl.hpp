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
#include "catch_test_spec.hpp"
#include "catch_test_case_tracker.hpp"
#include "catch_timer.h"
#include "catch_result_builder.h"
#include "catch_fatal_condition.hpp"

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
            m_prevRunner( m_context.getRunner() ),
            m_prevResultCapture( m_context.getResultCapture() ),
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

        Totals runTest( TestCase const& testCase ) {
            Totals prevTotals = m_totals;

            std::string redirectedCout;
            std::string redirectedCerr;

            TestCaseInfo testInfo = testCase.getTestCaseInfo();

            m_reporter->testCaseStarting( testInfo );

            m_activeTestCase = &testCase;
            m_testCaseTracker = TestCaseTracker( testInfo.name );

            do {
                do {
                    runCurrentTest( redirectedCout, redirectedCerr );
                }
                while( !m_testCaseTracker->isCompleted() && !aborting() );
            }
            while( getCurrentContext().advanceGeneratorsForCurrentTest() && !aborting() );

            Totals deltaTotals = m_totals.delta( prevTotals );
            m_totals.testCases += deltaTotals.testCases;
            m_reporter->testCaseEnded( TestCaseStats(   testInfo,
                                                        deltaTotals,
                                                        redirectedCout,
                                                        redirectedCerr,
                                                        aborting() ) );

            m_activeTestCase = NULL;
            m_testCaseTracker.reset();

            return deltaTotals;
        }

        Ptr<IConfig const> config() const {
            return m_config;
        }

    private: // IResultCapture


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
            m_lastResult = result;
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

        virtual void sectionEnded( SectionInfo const& info, Counts const& prevAssertions, double _durationInSeconds ) {
            if( std::uncaught_exception() ) {
                m_unfinishedSections.push_back( UnfinishedSections( info, prevAssertions, _durationInSeconds ) );
                return;
            }

            Counts assertions = m_totals.assertions - prevAssertions;
            bool missingAssertions = testForMissingAssertions( assertions );

            m_testCaseTracker->leaveSection();

            m_reporter->sectionEnded( SectionStats( info, assertions, _durationInSeconds, missingAssertions ) );
            m_messages.clear();
        }

        virtual void pushScopedMessage( MessageInfo const& message ) {
            m_messages.push_back( message );
        }

        virtual void popScopedMessage( MessageInfo const& message ) {
            m_messages.erase( std::remove( m_messages.begin(), m_messages.end(), message ), m_messages.end() );
        }

        virtual std::string getCurrentTestName() const {
            return m_activeTestCase
                ? m_activeTestCase->getTestCaseInfo().name
                : "";
        }

        virtual const AssertionResult* getLastResult() const {
            return &m_lastResult;
        }

        virtual void handleFatalErrorCondition( std::string const& message ) {
            ResultBuilder resultBuilder = makeUnexpectedResultBuilder();
            resultBuilder.setResultType( ResultWas::FatalErrorCondition );
            resultBuilder << message;
            resultBuilder.captureExpression();

            handleUnfinishedSections();

            // Recreate section for test case (as we will lose the one that was in scope)
            TestCaseInfo const& testCaseInfo = m_activeTestCase->getTestCaseInfo();
            SectionInfo testCaseSection( testCaseInfo.lineInfo, testCaseInfo.name, testCaseInfo.description );

            Counts assertions;
            assertions.failed = 1;
            SectionStats testCaseSectionStats( testCaseSection, assertions, 0, false );
            m_reporter->sectionEnded( testCaseSectionStats );

            TestCaseInfo testInfo = m_activeTestCase->getTestCaseInfo();

            Totals deltaTotals;
            deltaTotals.testCases.failed = 1;
            m_reporter->testCaseEnded( TestCaseStats(   testInfo,
                                                        deltaTotals,
                                                        "",
                                                        "",
                                                        false ) );
            m_totals.testCases.failed++;
            testGroupEnded( "", m_totals, 1, 1 );
            m_reporter->testRunEnded( TestRunStats( m_runInfo, m_totals, false ) );
        }

    public:
        // !TBD We need to do this another way!
        bool aborting() const {
            return m_totals.assertions.failed == static_cast<std::size_t>( m_config->abortAfter() );
        }

    private:

        void runCurrentTest( std::string& redirectedCout, std::string& redirectedCerr ) {
            TestCaseInfo const& testCaseInfo = m_activeTestCase->getTestCaseInfo();
            SectionInfo testCaseSection( testCaseInfo.lineInfo, testCaseInfo.name, testCaseInfo.description );
            m_reporter->sectionStarting( testCaseSection );
            Counts prevAssertions = m_totals.assertions;
            double duration = 0;
            try {
                m_lastAssertionInfo = AssertionInfo( "TEST_CASE", testCaseInfo.lineInfo, "", ResultDisposition::Normal );
                TestCaseTracker::Guard guard( *m_testCaseTracker );

                Timer timer;
                timer.start();
                if( m_reporter->getPreferences().shouldRedirectStdOut ) {
                    StreamRedirect coutRedir( Catch::cout(), redirectedCout );
                    StreamRedirect cerrRedir( Catch::cerr(), redirectedCerr );
                    invokeActiveTestCase();
                }
                else {
                    invokeActiveTestCase();
                }
                duration = timer.getElapsedSeconds();
            }
            catch( TestFailureException& ) {
                // This just means the test was aborted due to failure
            }
            catch(...) {
                makeUnexpectedResultBuilder().useActiveException();
            }
            handleUnfinishedSections();
            m_messages.clear();

            Counts assertions = m_totals.assertions - prevAssertions;
            bool missingAssertions = testForMissingAssertions( assertions );

            if( testCaseInfo.okToFail() ) {
                std::swap( assertions.failedButOk, assertions.failed );
                m_totals.assertions.failed -= assertions.failedButOk;
                m_totals.assertions.failedButOk += assertions.failedButOk;
            }

            SectionStats testCaseSectionStats( testCaseSection, assertions, duration, missingAssertions );
            m_reporter->sectionEnded( testCaseSectionStats );
        }

        void invokeActiveTestCase() {
            FatalConditionHandler fatalConditionHandler; // Handle signals
            m_activeTestCase->invoke();
            fatalConditionHandler.reset();
        }

    private:

        ResultBuilder makeUnexpectedResultBuilder() const {
            return ResultBuilder(   m_lastAssertionInfo.macroName.c_str(),
                                    m_lastAssertionInfo.lineInfo,
                                    m_lastAssertionInfo.capturedExpression.c_str(),
                                    m_lastAssertionInfo.resultDisposition );
        }

        void handleUnfinishedSections() {
            // If sections ended prematurely due to an exception we stored their
            // infos here so we can tear them down outside the unwind process.
            for( std::vector<UnfinishedSections>::const_reverse_iterator it = m_unfinishedSections.rbegin(),
                        itEnd = m_unfinishedSections.rend();
                    it != itEnd;
                    ++it )
                sectionEnded( it->info, it->prevAssertions, it->durationInSeconds );
            m_unfinishedSections.clear();
        }

        struct UnfinishedSections {
            UnfinishedSections( SectionInfo const& _info, Counts const& _prevAssertions, double _durationInSeconds )
            : info( _info ), prevAssertions( _prevAssertions ), durationInSeconds( _durationInSeconds )
            {}

            SectionInfo info;
            Counts prevAssertions;
            double durationInSeconds;
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

    IResultCapture& getResultCapture() {
        if( IResultCapture* capture = getCurrentContext().getResultCapture() )
            return *capture;
        else
            throw std::logic_error( "No result capture instance" );
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RUNNER_IMPL_HPP_INCLUDED
