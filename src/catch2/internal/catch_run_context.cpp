
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_run_context.hpp>

#include <catch2/interfaces/catch_interfaces_generatortracker.hpp>
#include <catch2/interfaces/catch_interfaces_config.hpp>
#include <catch2/internal/catch_compiler_capabilities.hpp>
#include <catch2/internal/catch_context.hpp>
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_fatal_condition_handler.hpp>
#include <catch2/internal/catch_random_number_generator.hpp>
#include <catch2/catch_timer.hpp>
#include <catch2/internal/catch_output_redirect.hpp>
#include <catch2/internal/catch_assertion_handler.hpp>
#include <catch2/internal/catch_test_failure_exception.hpp>

#include <cassert>
#include <algorithm>

namespace Catch {

    namespace Generators {
        struct GeneratorTracker : TestCaseTracking::TrackerBase, IGeneratorTracker {
            GeneratorBasePtr m_generator;

            GeneratorTracker( TestCaseTracking::NameAndLocation const& nameAndLocation, TrackerContext& ctx, ITracker* parent )
            :   TrackerBase( nameAndLocation, ctx, parent )
            {}
            ~GeneratorTracker();

            static GeneratorTracker& acquire( TrackerContext& ctx, TestCaseTracking::NameAndLocation const& nameAndLocation ) {
                GeneratorTracker* tracker;

                ITracker& currentTracker = ctx.currentTracker();
                // Under specific circumstances, the generator we want
                // to acquire is also the current tracker. If this is
                // the case, we have to avoid looking through current
                // tracker's children, and instead return the current
                // tracker.
                // A case where this check is important is e.g.
                //     for (int i = 0; i < 5; ++i) {
                //         int n = GENERATE(1, 2);
                //     }
                //
                // without it, the code above creates 5 nested generators.
                if ( currentTracker.nameAndLocation() == nameAndLocation ) {
                    auto thisTracker =
                        currentTracker.parent()->findChild( nameAndLocation );
                    assert( thisTracker );
                    assert( thisTracker->isGeneratorTracker() );
                    tracker = static_cast<GeneratorTracker*>( thisTracker );
                } else if ( ITracker* childTracker =
                                currentTracker.findChild( nameAndLocation ) ) {
                    assert( childTracker );
                    assert( childTracker->isGeneratorTracker() );
                    tracker = static_cast<GeneratorTracker*>( childTracker );
                } else {
                    auto newTracker =
                        Catch::Detail::make_unique<GeneratorTracker>(
                            nameAndLocation, ctx, &currentTracker );
                    tracker = newTracker.get();
                    currentTracker.addChild( CATCH_MOVE(newTracker) );
                }

                if( !tracker->isComplete() ) {
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
                // If a generator has a child (it is followed by a section)
                // and none of its children have started, then we must wait
                // until later to start consuming its values.
                // This catches cases where `GENERATE` is placed between two
                // `SECTION`s.
                // **The check for m_children.empty cannot be removed**.
                // doing so would break `GENERATE` _not_ followed by `SECTION`s.
                const bool should_wait_for_child = [&]() {
                    // No children -> nobody to wait for
                    if ( m_children.empty() ) {
                        return false;
                    }
                    // If at least one child started executing, don't wait
                    if ( std::find_if(
                             m_children.begin(),
                             m_children.end(),
                             []( TestCaseTracking::ITrackerPtr const& tracker ) {
                                 return tracker->hasStarted();
                             } ) != m_children.end() ) {
                        return false;
                    }

                    // No children have started. We need to check if they _can_
                    // start, and thus we should wait for them, or they cannot
                    // start (due to filters), and we shouldn't wait for them
                    ITracker* parent = m_parent;
                    // This is safe: there is always at least one section
                    // tracker in a test case tracking tree
                    while ( !parent->isSectionTracker() ) {
                        parent = parent->parent();
                    }
                    assert( parent &&
                            "Missing root (test case) level section" );

                    auto const& parentSection =
                        static_cast<SectionTracker const&>( *parent );
                    auto const& filters = parentSection.getFilters();
                    // No filters -> no restrictions on running sections
                    if ( filters.empty() ) {
                        return true;
                    }

                    for ( auto const& child : m_children ) {
                        if ( child->isSectionTracker() &&
                             std::find(
                                 filters.begin(),
                                 filters.end(),
                                 static_cast<SectionTracker const&>( *child )
                                     .trimmedName() ) != filters.end() ) {
                            return true;
                        }
                    }
                    return false;
                }();

                // This check is a bit tricky, because m_generator->next()
                // has a side-effect, where it consumes generator's current
                // value, but we do not want to invoke the side-effect if
                // this generator is still waiting for any child to start.
                if ( should_wait_for_child ||
                     ( m_runState == CompletedSuccessfully &&
                       m_generator->next() ) ) {
                    m_children.clear();
                    m_runState = Executing;
                }
            }

            // IGeneratorTracker interface
            auto getGenerator() const -> GeneratorBasePtr const& override {
                return m_generator;
            }
            void setGenerator( GeneratorBasePtr&& generator ) override {
                m_generator = CATCH_MOVE( generator );
            }
        };
        GeneratorTracker::~GeneratorTracker() = default;
    }

    RunContext::RunContext(IConfig const* _config, IStreamingReporterPtr&& reporter)
    :   m_runInfo(_config->name()),
        m_context(getCurrentMutableContext()),
        m_config(_config),
        m_reporter(CATCH_MOVE(reporter)),
        m_lastAssertionInfo{ StringRef(), SourceLineInfo("",0), StringRef(), ResultDisposition::Normal },
        m_includeSuccessfulResults( m_config->includeSuccessfulResults() || m_reporter->getPreferences().shouldReportAllAssertions )
    {
        m_context.setResultCapture(this);
        m_reporter->testRunStarting(m_runInfo);
    }

    RunContext::~RunContext() {
        m_reporter->testRunEnded(TestRunStats(m_runInfo, m_totals, aborting()));
    }

    Totals RunContext::runTest(TestCaseHandle const& testCase) {
        const Totals prevTotals = m_totals;

        std::string redirectedCout;
        std::string redirectedCerr;

        auto const& testInfo = testCase.getTestCaseInfo();

        m_reporter->testCaseStarting(testInfo);

        m_activeTestCase = &testCase;


        ITracker& rootTracker = m_trackerContext.startRun();
        assert(rootTracker.isSectionTracker());
        static_cast<SectionTracker&>(rootTracker).addInitialFilters(m_config->getSectionsToRun());

        uint64_t testRuns = 0;
        do {
            m_trackerContext.startCycle();
            m_testCaseTracker = &SectionTracker::acquire(m_trackerContext, TestCaseTracking::NameAndLocation(testInfo.name, testInfo.lineInfo));

            m_reporter->testCasePartialStarting(testInfo, testRuns);

            const auto beforeRunTotals = m_totals;
            std::string oneRunCout, oneRunCerr;
            runCurrentTest(oneRunCout, oneRunCerr);
            redirectedCout += oneRunCout;
            redirectedCerr += oneRunCerr;

            const auto singleRunTotals = m_totals.delta(beforeRunTotals);
            auto statsForOneRun = TestCaseStats(testInfo, singleRunTotals, redirectedCout, oneRunCerr, aborting());

            m_reporter->testCasePartialEnded(statsForOneRun, testRuns);
            ++testRuns;
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


    void RunContext::assertionEnded(AssertionResult const & result) {
        if (result.getResultType() == ResultWas::Ok) {
            m_totals.assertions.passed++;
            m_lastAssertionPassed = true;
        } else if (!result.succeeded()) {
            m_lastAssertionPassed = false;
            if (result.isOk()) {
            }
            else if( m_activeTestCase->getTestCaseInfo().okToFail() )
                m_totals.assertions.failedButOk++;
            else
                m_totals.assertions.failed++;
        }
        else {
            m_lastAssertionPassed = true;
        }

        m_reporter->assertionEnded(AssertionStats(result, m_messages, m_totals));

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
    auto RunContext::acquireGeneratorTracker( StringRef generatorName, SourceLineInfo const& lineInfo ) -> IGeneratorTracker& {
        using namespace Generators;
        GeneratorTracker& tracker = GeneratorTracker::acquire(m_trackerContext,
                                                              TestCaseTracking::NameAndLocation( static_cast<std::string>(generatorName), lineInfo ) );
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

    void RunContext::benchmarkPreparing( StringRef name ) {
        m_reporter->benchmarkPreparing(name);
    }
    void RunContext::benchmarkStarting( BenchmarkInfo const& info ) {
        m_reporter->benchmarkStarting( info );
    }
    void RunContext::benchmarkEnded( BenchmarkStats<> const& stats ) {
        m_reporter->benchmarkEnded( stats );
    }
    void RunContext::benchmarkFailed( StringRef error ) {
        m_reporter->benchmarkFailed( error );
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
        tempResult.message = static_cast<std::string>(message);
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
        // We need to engage a handler for signals/structured exceptions
        // before running the tests themselves, or the binary can crash
        // without failed test being reported.
        FatalConditionHandlerGuard _(&m_fatalConditionhandler);
        m_activeTestCase->invoke();
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
            StringRef message,
            AssertionReaction& reaction
    ) {
        m_reporter->assertionStarting( info );

        m_lastAssertionInfo = info;

        AssertionResultData data( resultType, LazyExpression( false ) );
        data.message = static_cast<std::string>(message);
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

    void seedRng(IConfig const& config) {
        if (config.rngSeed() != 0) {
            std::srand(config.rngSeed());
            rng().seed(config.rngSeed());
        }
    }

    unsigned int rngSeed() {
        return getCurrentContext().getConfig()->rngSeed();
    }

}
