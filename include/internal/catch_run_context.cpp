#include "catch_run_context.h"
#include "catch_compiler_capabilities.h"
#include "catch_context.h"
#include "catch_enforce.h"
#include "catch_random_number_generator.h"
#include "catch_stream.h"
#include "catch_output_redirect.h"

#include <cassert>
#include <algorithm>
#include <sstream>

namespace Catch {

    namespace Generators {
        struct GeneratorTracker : TestCaseTracking::TrackerBase, IGeneratorTracker {
            GeneratorBasePtr m_generator;

            GeneratorTracker( TestCaseTracking::NameAndLocation const& nameAndLocation, TrackerContext& ctx, ITracker* parent )
            :   TrackerBase( nameAndLocation, ctx, parent )
            {}
            ~GeneratorTracker();

            static GeneratorTracker& acquire( TrackerContext& ctx, TestCaseTracking::NameAndLocation const& nameAndLocation ) {
                std::shared_ptr<GeneratorTracker> tracker;

                ITracker& currentTracker = ctx.currentTracker();
                if( TestCaseTracking::ITrackerPtr childTracker = currentTracker.findChild( nameAndLocation ) ) {
                    assert( childTracker );
                    assert( childTracker->isGeneratorTracker() );
                    tracker = std::static_pointer_cast<GeneratorTracker>( childTracker );
                }
                else {
                    tracker = std::make_shared<GeneratorTracker>( nameAndLocation, ctx, &currentTracker );
                    currentTracker.addChild( tracker );
                }

                if( !ctx.completedCycle() && !tracker->isComplete() ) {
                    tracker->open();
                }

                return *tracker;
            }

            // TrackerBase interface
            bool isGeneratorTracker() const override { return true; }
            auto hasGenerator() const -> bool override {
                return !!m_generator;
            }
            void close() override {
                TrackerBase::close();
                // Generator interface only finds out if it has another item on atual move
                if (m_runState == CompletedSuccessfully && m_generator->next()) {
                    m_children.clear();
                    m_runState = Executing;
                }
            }

            // IGeneratorTracker interface
            auto getGenerator() const -> GeneratorBasePtr const& override {
                return m_generator;
            }
            void setGenerator( GeneratorBasePtr&& generator ) override {
                m_generator = std::move( generator );
            }
        };
        GeneratorTracker::~GeneratorTracker() {}
    }

    RunContext::RunContext(IConfigPtr const& _config, IStreamingReporterPtr&& reporter)
    :   m_runInfo(_config->name()),
        m_context(getCurrentMutableContext()),
        m_config(_config),
        m_reporter(std::move(reporter)),
        m_lastAssertionInfo{ StringRef(), SourceLineInfo("",0), StringRef(), ResultDisposition::Normal },
        m_includeSuccessfulResults( m_config->includeSuccessfulResults() || m_reporter->getPreferences().shouldReportAllAssertions )
    {
        m_context.setRunner(this);
        m_context.setConfig(m_config);
        m_context.setResultCapture(this);
        m_reporter->testRunStarting(m_runInfo);
    }

    RunContext::~RunContext() {
        m_reporter->testRunEnded(TestRunStats(m_runInfo, m_totals, aborting()));
    }

    void RunContext::testGroupStarting(std::string const& testSpec, std::size_t groupIndex, std::size_t groupsCount) {
        m_reporter->testGroupStarting(GroupInfo(testSpec, groupIndex, groupsCount));
    }

    void RunContext::testGroupEnded(std::string const& testSpec, Totals const& totals, std::size_t groupIndex, std::size_t groupsCount) {
        m_reporter->testGroupEnded(TestGroupStats(GroupInfo(testSpec, groupIndex, groupsCount), totals, aborting()));
    }

    Totals RunContext::runTest(TestCase const& testCase) {
        Totals prevTotals = m_totals;

        std::string redirectedCout;
        std::string redirectedCerr;

        auto const& testInfo = testCase.getTestCaseInfo();

        m_reporter->testCaseStarting(testInfo);

        m_activeTestCase = &testCase;


        ITracker& rootTracker = m_trackerContext.startRun();
        assert(rootTracker.isSectionTracker());
        static_cast<SectionTracker&>(rootTracker).addInitialFilters(m_config->getSectionsToRun());
        do {
            m_trackerContext.startCycle();
            m_testCaseTracker = &SectionTracker::acquire(m_trackerContext, TestCaseTracking::NameAndLocation(testInfo.name, testInfo.lineInfo));
            runCurrentTest(redirectedCout, redirectedCerr);
        } while (!m_testCaseTracker->isSuccessfullyCompleted() && !aborting());

        Totals deltaTotals = m_totals.delta(prevTotals);
        if (testInfo.expectedToFail() && deltaTotals.testCases.passed > 0) {
            deltaTotals.assertions.failed++;
            deltaTotals.testCases.passed--;
            deltaTotals.testCases.failed++;
        }
        m_totals.testCases += deltaTotals.testCases;
        m_reporter->testCaseEnded(TestCaseStats(testInfo,
                                  deltaTotals,
                                  redirectedCout,
                                  redirectedCerr,
                                  aborting()));

        m_activeTestCase = nullptr;
        m_testCaseTracker = nullptr;

        return deltaTotals;
    }

    IConfigPtr RunContext::config() const {
        return m_config;
    }

    IStreamingReporter& RunContext::reporter() const {
        return *m_reporter;
    }

    void RunContext::assertionEnded(AssertionResult const & result) {
        if (result.getResultType() == ResultWas::Ok) {
            m_totals.assertions.passed++;
            m_lastAssertionPassed = true;
        } else if (!result.isOk()) {
            m_lastAssertionPassed = false;
            if( m_activeTestCase->getTestCaseInfo().okToFail() )
                m_totals.assertions.failedButOk++;
            else
                m_totals.assertions.failed++;
        }
        else {
            m_lastAssertionPassed = true;
        }

        // We have no use for the return value (whether messages should be cleared), because messages were made scoped
        // and should be let to clear themselves out.
        static_cast<void>(m_reporter->assertionEnded(AssertionStats(result, m_messages, m_totals)));

        if (result.getResultType() != ResultWas::Warning)
            m_messageScopes.clear();

        // Reset working state
        resetAssertionInfo();
        m_lastResult = result;
    }
    void RunContext::resetAssertionInfo() {
        m_lastAssertionInfo.macroName = StringRef();
        m_lastAssertionInfo.capturedExpression = "{Unknown expression after the reported line}"_sr;
    }

    bool RunContext::sectionStarted(SectionInfo const & sectionInfo, Counts & assertions) {
        ITracker& sectionTracker = SectionTracker::acquire(m_trackerContext, TestCaseTracking::NameAndLocation(sectionInfo.name, sectionInfo.lineInfo));
        if (!sectionTracker.isOpen())
            return false;
        m_activeSections.push_back(&sectionTracker);

        m_lastAssertionInfo.lineInfo = sectionInfo.lineInfo;

        m_reporter->sectionStarting(sectionInfo);

        assertions = m_totals.assertions;

        return true;
    }
    auto RunContext::acquireGeneratorTracker( SourceLineInfo const& lineInfo ) -> IGeneratorTracker& {
        using namespace Generators;
        GeneratorTracker& tracker = GeneratorTracker::acquire( m_trackerContext, TestCaseTracking::NameAndLocation( "generator", lineInfo ) );
        assert( tracker.isOpen() );
        m_lastAssertionInfo.lineInfo = lineInfo;
        return tracker;
    }

    bool RunContext::testForMissingAssertions(Counts& assertions) {
        if (assertions.total() != 0)
            return false;
        if (!m_config->warnAboutMissingAssertions())
            return false;
        if (m_trackerContext.currentTracker().hasChildren())
            return false;
        m_totals.assertions.failed++;
        assertions.failed++;
        return true;
    }

    void RunContext::sectionEnded(SectionEndInfo const & endInfo) {
        Counts assertions = m_totals.assertions - endInfo.prevAssertions;
        bool missingAssertions = testForMissingAssertions(assertions);

        if (!m_activeSections.empty()) {
            m_activeSections.back()->close();
            m_activeSections.pop_back();
        }

        m_reporter->sectionEnded(SectionStats(endInfo.sectionInfo, assertions, endInfo.durationInSeconds, missingAssertions));
        m_messages.clear();
        m_messageScopes.clear();
    }

    void RunContext::sectionEndedEarly(SectionEndInfo const & endInfo) {
        if (m_unfinishedSections.empty())
            m_activeSections.back()->fail();
        else
            m_activeSections.back()->close();
        m_activeSections.pop_back();

        m_unfinishedSections.push_back(endInfo);
    }
    void RunContext::benchmarkStarting( BenchmarkInfo const& info ) {
        m_reporter->benchmarkStarting( info );
    }
    void RunContext::benchmarkEnded( BenchmarkStats const& stats ) {
        m_reporter->benchmarkEnded( stats );
    }

    void RunContext::pushScopedMessage(MessageInfo const & message) {
        m_messages.push_back(message);
    }

    void RunContext::popScopedMessage(MessageInfo const & message) {
        m_messages.erase(std::remove(m_messages.begin(), m_messages.end(), message), m_messages.end());
    }

    void RunContext::emplaceUnscopedMessage( MessageBuilder const& builder ) {
        m_messageScopes.emplace_back( builder );
    }

    std::string RunContext::getCurrentTestName() const {
        return m_activeTestCase
            ? m_activeTestCase->getTestCaseInfo().name
            : std::string();
    }

    const AssertionResult * RunContext::getLastResult() const {
        return &(*m_lastResult);
    }

    void RunContext::exceptionEarlyReported() {
        m_shouldReportUnexpected = false;
    }

    void RunContext::handleFatalErrorCondition( StringRef message ) {
        // First notify reporter that bad things happened
        m_reporter->fatalErrorEncountered(message);

        // Don't rebuild the result -- the stringification itself can cause more fatal errors
        // Instead, fake a result data.
        AssertionResultData tempResult( ResultWas::FatalErrorCondition, { false } );
        tempResult.message = message;
        AssertionResult result(m_lastAssertionInfo, tempResult);

        assertionEnded(result);

        handleUnfinishedSections();

        // Recreate section for test case (as we will lose the one that was in scope)
        auto const& testCaseInfo = m_activeTestCase->getTestCaseInfo();
        SectionInfo testCaseSection(testCaseInfo.lineInfo, testCaseInfo.name);

        Counts assertions;
        assertions.failed = 1;
        SectionStats testCaseSectionStats(testCaseSection, assertions, 0, false);
        m_reporter->sectionEnded(testCaseSectionStats);

        auto const& testInfo = m_activeTestCase->getTestCaseInfo();

        Totals deltaTotals;
        deltaTotals.testCases.failed = 1;
        deltaTotals.assertions.failed = 1;
        m_reporter->testCaseEnded(TestCaseStats(testInfo,
                                  deltaTotals,
                                  std::string(),
                                  std::string(),
                                  false));
        m_totals.testCases.failed++;
        testGroupEnded(std::string(), m_totals, 1, 1);
        m_reporter->testRunEnded(TestRunStats(m_runInfo, m_totals, false));
    }

    bool RunContext::lastAssertionPassed() {
         return m_lastAssertionPassed;
    }

    void RunContext::assertionPassed() {
        m_lastAssertionPassed = true;
        ++m_totals.assertions.passed;
        resetAssertionInfo();
        m_messageScopes.clear();
    }

    bool RunContext::aborting() const {
        return m_totals.assertions.failed >= static_cast<std::size_t>(m_config->abortAfter());
    }

    void RunContext::runCurrentTest(std::string & redirectedCout, std::string & redirectedCerr) {
        auto const& testCaseInfo = m_activeTestCase->getTestCaseInfo();
        SectionInfo testCaseSection(testCaseInfo.lineInfo, testCaseInfo.name);
        m_reporter->sectionStarting(testCaseSection);
        Counts prevAssertions = m_totals.assertions;
        double duration = 0;
        m_shouldReportUnexpected = true;
        m_lastAssertionInfo = { "TEST_CASE"_sr, testCaseInfo.lineInfo, StringRef(), ResultDisposition::Normal };

        seedRng(*m_config);

        Timer timer;
        CATCH_TRY {
            if (m_reporter->getPreferences().shouldRedirectStdOut) {
#if !defined(CATCH_CONFIG_EXPERIMENTAL_REDIRECT)
                RedirectedStreams redirectedStreams(redirectedCout, redirectedCerr);

                timer.start();
                invokeActiveTestCase();
#else
                OutputRedirect r(redirectedCout, redirectedCerr);
                timer.start();
                invokeActiveTestCase();
#endif
            } else {
                timer.start();
                invokeActiveTestCase();
            }
            duration = timer.getElapsedSeconds();
        } CATCH_CATCH_ANON (TestFailureException&) {
            // This just means the test was aborted due to failure
        } CATCH_CATCH_ALL {
            // Under CATCH_CONFIG_FAST_COMPILE, unexpected exceptions under REQUIRE assertions
            // are reported without translation at the point of origin.
            if( m_shouldReportUnexpected ) {
                AssertionReaction dummyReaction;
                handleUnexpectedInflightException( m_lastAssertionInfo, translateActiveException(), dummyReaction );
            }
        }
        Counts assertions = m_totals.assertions - prevAssertions;
        bool missingAssertions = testForMissingAssertions(assertions);

        m_testCaseTracker->close();
        handleUnfinishedSections();
        m_messages.clear();
        m_messageScopes.clear();

        SectionStats testCaseSectionStats(testCaseSection, assertions, duration, missingAssertions);
        m_reporter->sectionEnded(testCaseSectionStats);
    }

    void RunContext::invokeActiveTestCase() {
        FatalConditionHandler fatalConditionHandler; // Handle signals
        m_activeTestCase->invoke();
        fatalConditionHandler.reset();
    }

    void RunContext::handleUnfinishedSections() {
        // If sections ended prematurely due to an exception we stored their
        // infos here so we can tear them down outside the unwind process.
        for (auto it = m_unfinishedSections.rbegin(),
             itEnd = m_unfinishedSections.rend();
             it != itEnd;
             ++it)
            sectionEnded(*it);
        m_unfinishedSections.clear();
    }

    void RunContext::handleExpr(
        AssertionInfo const& info,
        ITransientExpression const& expr,
        AssertionReaction& reaction
    ) {
        m_reporter->assertionStarting( info );

        bool negated = isFalseTest( info.resultDisposition );
        bool result = expr.getResult() != negated;

        if( result ) {
            if (!m_includeSuccessfulResults) {
                assertionPassed();
            }
            else {
                reportExpr(info, ResultWas::Ok, &expr, negated);
            }
        }
        else {
            reportExpr(info, ResultWas::ExpressionFailed, &expr, negated );
            populateReaction( reaction );
        }
    }
    void RunContext::reportExpr(
            AssertionInfo const &info,
            ResultWas::OfType resultType,
            ITransientExpression const *expr,
            bool negated ) {

        m_lastAssertionInfo = info;
        AssertionResultData data( resultType, LazyExpression( negated ) );

        AssertionResult assertionResult{ info, data };
        assertionResult.m_resultData.lazyExpression.m_transientExpression = expr;

        assertionEnded( assertionResult );
    }

    void RunContext::handleMessage(
            AssertionInfo const& info,
            ResultWas::OfType resultType,
            StringRef const& message,
            AssertionReaction& reaction
    ) {
        m_reporter->assertionStarting( info );

        m_lastAssertionInfo = info;

        AssertionResultData data( resultType, LazyExpression( false ) );
        data.message = message;
        AssertionResult assertionResult{ m_lastAssertionInfo, data };
        assertionEnded( assertionResult );
        if( !assertionResult.isOk() )
            populateReaction( reaction );
    }
    void RunContext::handleUnexpectedExceptionNotThrown(
            AssertionInfo const& info,
            AssertionReaction& reaction
    ) {
        handleNonExpr(info, Catch::ResultWas::DidntThrowException, reaction);
    }

    void RunContext::handleUnexpectedInflightException(
            AssertionInfo const& info,
            std::string const& message,
            AssertionReaction& reaction
    ) {
        m_lastAssertionInfo = info;

        AssertionResultData data( ResultWas::ThrewException, LazyExpression( false ) );
        data.message = message;
        AssertionResult assertionResult{ info, data };
        assertionEnded( assertionResult );
        populateReaction( reaction );
    }

    void RunContext::populateReaction( AssertionReaction& reaction ) {
        reaction.shouldDebugBreak = m_config->shouldDebugBreak();
        reaction.shouldThrow = aborting() || (m_lastAssertionInfo.resultDisposition & ResultDisposition::Normal);
    }

    void RunContext::handleIncomplete(
            AssertionInfo const& info
    ) {
        m_lastAssertionInfo = info;

        AssertionResultData data( ResultWas::ThrewException, LazyExpression( false ) );
        data.message = "Exception translation was disabled by CATCH_CONFIG_FAST_COMPILE";
        AssertionResult assertionResult{ info, data };
        assertionEnded( assertionResult );
    }
    void RunContext::handleNonExpr(
            AssertionInfo const &info,
            ResultWas::OfType resultType,
            AssertionReaction &reaction
    ) {
        m_lastAssertionInfo = info;

        AssertionResultData data( resultType, LazyExpression( false ) );
        AssertionResult assertionResult{ info, data };
        assertionEnded( assertionResult );

        if( !assertionResult.isOk() )
            populateReaction( reaction );
    }


    IResultCapture& getResultCapture() {
        if (auto* capture = getCurrentContext().getResultCapture())
            return *capture;
        else
            CATCH_INTERNAL_ERROR("No result capture instance");
    }
}
