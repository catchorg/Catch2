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
#include "catch_test_registry.h"
#include "catch_test_case_info.h"
#include "catch_capture.hpp"
#include "catch_totals.h"
#include "catch_test_spec.h"
#include "catch_test_case_tracker.h"
#include "catch_timer.h"
#include "catch_assertionhandler.h"
#include "catch_fatal_condition.h"

#include <string>

namespace Catch {

    struct IMutableContext;

    class StreamRedirect {

    public:
        StreamRedirect(std::ostream& stream, std::string& targetString);
        ~StreamRedirect();

    private:
        std::ostream& m_stream;
        std::streambuf* m_prevBuf;
        ReusableStringStream m_oss;
        std::string& m_targetString;
    };

    // StdErr has two constituent streams in C++, std::cerr and std::clog
    // This means that we need to redirect 2 streams into 1 to keep proper
    // order of writes and cannot use StreamRedirect on its own
    class StdErrRedirect {
    public:
        explicit StdErrRedirect( std::string& targetString );
        ~StdErrRedirect();
    private:
        std::streambuf* m_cerrBuf;
        std::streambuf* m_clogBuf;
        ReusableStringStream m_oss;
        std::string& m_targetString;
    };

    ///////////////////////////////////////////////////////////////////////////

    class RunContext : public IResultCapture, public IRunner {

    public:
        RunContext( RunContext const& ) = delete;
        RunContext& operator =( RunContext const& ) = delete;

        explicit RunContext(IConfigPtr const& _config, IStreamingReporterPtr&& reporter);

        ~RunContext() override;

        void testGroupStarting(std::string const& testSpec, std::size_t groupIndex, std::size_t groupsCount);
        void testGroupEnded(std::string const& testSpec, Totals const& totals, std::size_t groupIndex, std::size_t groupsCount);

        Totals runTest(TestCase const& testCase);

        IConfigPtr config() const;
        IStreamingReporter& reporter() const;

        // Assertion handlers
        void handleExpr
                (   AssertionInfo const& info,
                    ITransientExpression const& expr,
                    AssertionReaction& reaction );
        void handleMessage
                (   AssertionInfo const& info,
                    ResultWas::OfType resultType,
                    StringRef const& message,
                    AssertionReaction& reaction );
        void handleUnexpectedExceptionNotThrown
                (   AssertionInfo const& info,
                    AssertionReaction& reaction );
        void handleUnexpectedInflightException
                (   AssertionInfo const& info,
                    std::string const& message,
                    AssertionReaction& reaction );
        void handleIncomplete
                (   AssertionInfo const& info );
        void handleNonExpr
                (   AssertionInfo const &info,
                    ResultWas::OfType resultType,
                    AssertionReaction &reaction );

        void reportExpr
                (AssertionInfo const &info,
                 ResultWas::OfType resultType,
                 ITransientExpression const *expr,
                 bool negated );

        void populateReaction( AssertionReaction& reaction );

    public: // IResultCapture

        void assertionEnded(AssertionResult const& result);

        bool sectionStarted( SectionInfo const& sectionInfo, Counts& assertions ) override;
        bool testForMissingAssertions(Counts& assertions);

        void sectionEnded(SectionEndInfo const& endInfo) override;
        void sectionEndedEarly(SectionEndInfo const& endInfo) override;

        void benchmarkStarting( BenchmarkInfo const& info ) override;
        void benchmarkEnded( BenchmarkStats const& stats ) override;

        void pushScopedMessage(MessageInfo const& message) override;
        void popScopedMessage(MessageInfo const& message) override;

        std::string getCurrentTestName() const override;

        const AssertionResult* getLastResult() const override;

        void exceptionEarlyReported() override;

        void handleFatalErrorCondition( StringRef message ) override;

        bool lastAssertionPassed() override;

        void assertionPassed() override;

    public:
        // !TBD We need to do this another way!
        bool aborting() const override;

    private:

        void runCurrentTest(std::string& redirectedCout, std::string& redirectedCerr);
        void invokeActiveTestCase();

        void resetAssertionInfo();

    private:

        void handleUnfinishedSections();

        TestRunInfo m_runInfo;
        IMutableContext& m_context;
        TestCase const* m_activeTestCase = nullptr;
        ITracker* m_testCaseTracker;
        Option<AssertionResult> m_lastResult;

        IConfigPtr m_config;
        Totals m_totals;
        IStreamingReporterPtr m_reporter;
        std::vector<MessageInfo> m_messages;
        AssertionInfo m_lastAssertionInfo;
        std::vector<SectionEndInfo> m_unfinishedSections;
        std::vector<ITracker*> m_activeSections;
        TrackerContext m_trackerContext;
        bool m_lastAssertionPassed = false;
        bool m_shouldReportUnexpected = true;
        bool m_includeSuccessfulResults;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RUNNER_IMPL_HPP_INCLUDED
