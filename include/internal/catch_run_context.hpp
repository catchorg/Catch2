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
#include "catch_fatal_condition.h"

#include <string>

namespace Catch {

    class StreamRedirect {

    public:
        StreamRedirect(std::ostream& stream, std::string& targetString);

        ~StreamRedirect();

    private:
        std::ostream& m_stream;
        std::streambuf* m_prevBuf;
        std::ostringstream m_oss;
        std::string& m_targetString;
    };

    ///////////////////////////////////////////////////////////////////////////

    class RunContext : public IResultCapture, public IRunner {

    public:
        RunContext( RunContext const& ) = delete;
        RunContext& operator =( RunContext const& ) = delete;

        explicit RunContext(IConfigPtr const& _config, IStreamingReporterPtr&& reporter);

        virtual ~RunContext();

        void testGroupStarting(std::string const& testSpec, std::size_t groupIndex, std::size_t groupsCount);
        void testGroupEnded(std::string const& testSpec, Totals const& totals, std::size_t groupIndex, std::size_t groupsCount);

        Totals runTest(TestCase const& testCase);

        IConfigPtr config() const;
        IStreamingReporter& reporter() const;

    private: // IResultCapture


        virtual void assertionEnded(AssertionResult const& result) override;

        virtual bool sectionStarted( SectionInfo const& sectionInfo, Counts& assertions ) override;
        bool testForMissingAssertions(Counts& assertions);

        virtual void sectionEnded(SectionEndInfo const& endInfo) override;

        virtual void sectionEndedEarly(SectionEndInfo const& endInfo) override;

        virtual void pushScopedMessage(MessageInfo const& message) override;

        virtual void popScopedMessage(MessageInfo const& message) override;

        virtual std::string getCurrentTestName() const override;

        virtual const AssertionResult* getLastResult() const override;

        virtual void exceptionEarlyReported() override;

        virtual void handleFatalErrorCondition(std::string const& message) override;

        virtual bool lastAssertionPassed() override;

        virtual void assertionPassed();

        virtual void assertionRun();

    public:
        // !TBD We need to do this another way!
        bool aborting() const override;

    private:

        void runCurrentTest(std::string& redirectedCout, std::string& redirectedCerr);
        void invokeActiveTestCase();

    private:

        ResultBuilder makeUnexpectedResultBuilder() const;

        void handleUnfinishedSections();

        TestRunInfo m_runInfo;
        IMutableContext& m_context;
        TestCase const* m_activeTestCase = nullptr;
        ITracker* m_testCaseTracker;
        AssertionResult m_lastResult;

        IConfigPtr m_config;
        Totals m_totals;
        IStreamingReporterPtr m_reporter;
        std::vector<MessageInfo> m_messages;
        AssertionInfo m_lastAssertionInfo;
        std::vector<SectionEndInfo> m_unfinishedSections;
        std::vector<ITracker*> m_activeSections;
        TrackerContext m_trackerContext;
        size_t m_prevPassed = 0;
        bool m_shouldReportUnexpected = true;
    };

    IResultCapture& getResultCapture();

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RUNNER_IMPL_HPP_INCLUDED
