
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_RUN_CONTEXT_HPP_INCLUDED
#define CATCH_RUN_CONTEXT_HPP_INCLUDED

#include <catch2/catch_test_run_info.hpp>
#include <catch2/catch_totals.hpp>
#include <catch2/catch_assertion_info.hpp>
#include <catch2/internal/catch_benchmark_stats_fwd.hpp>
#include <catch2/internal/catch_unique_ptr.hpp>

#include <string>
#include <chrono>

namespace Catch {

    class TestCaseHandle;
    class AssertionResult;
    struct AssertionInfo;
    struct SectionInfo;
    struct SectionEndInfo;
    struct MessageInfo;
    struct MessageBuilder;
    struct Counts;
    struct AssertionReaction;
    struct SourceLineInfo;

    class ITransientExpression;
    class IGeneratorTracker;

    struct BenchmarkInfo;

    namespace Generators {
        class GeneratorUntypedBase;
        using GeneratorBasePtr = Catch::Detail::unique_ptr<GeneratorUntypedBase>;
    }


    class IGeneratorTracker;
    class IConfig;
    // Fixme: Take out the namespace?
    namespace TestCaseTracking {
        class ITracker;
    }
    using TestCaseTracking::ITracker;

    class IEventListener;
    using IEventListenerPtr = Detail::unique_ptr<IEventListener>;

    class RunContext {
        struct RunContextImpl;
        Detail::unique_ptr<RunContextImpl> m_impl;
    public:
        RunContext( RunContext const& ) = delete;
        RunContext& operator =( RunContext const& ) = delete;

        explicit RunContext( IConfig const* _config, IEventListenerPtr&& reporter );

        ~RunContext();

        Totals runTest(TestCaseHandle const& testCase);

    public: // IResultCapture

        // Assertion handlers
        void handleExpr
                (   AssertionInfo const& info,
                    ITransientExpression const& expr,
                    AssertionReaction& reaction );
        void handleMessage
                (   AssertionInfo const& info,
                    ResultWas::OfType resultType,
                    StringRef message,
                    AssertionReaction& reaction );
        void handleUnexpectedExceptionNotThrown
                (   AssertionInfo const& info,
                    AssertionReaction& reaction );
        void handleUnexpectedInflightException
                (   AssertionInfo const& info,
                    std::string&& message,
                    AssertionReaction& reaction );
        void handleIncomplete
                (   AssertionInfo const& info );
        void handleNonExpr
                (   AssertionInfo const &info,
                    ResultWas::OfType resultType,
                    AssertionReaction &reaction );

        bool sectionStarted( StringRef sectionName,
                             SourceLineInfo const& sectionLineInfo,
                             Counts& assertions );

        void sectionEnded( SectionEndInfo&& endInfo );
        void sectionEndedEarly( SectionEndInfo&& endInfo );

        IGeneratorTracker*
        acquireGeneratorTracker( StringRef generatorName,
                                 SourceLineInfo const& lineInfo );
        IGeneratorTracker* createGeneratorTracker(
            StringRef generatorName,
            SourceLineInfo lineInfo,
            Generators::GeneratorBasePtr&& generator );


        void benchmarkPreparing( StringRef name );
        void benchmarkStarting( BenchmarkInfo const& info );
        void benchmarkEnded( BenchmarkStats<> const& stats );
        void benchmarkFailed( StringRef error );

        void pushScopedMessage( MessageInfo const& message );
        void popScopedMessage( MessageInfo const& message );

        void emplaceUnscopedMessage( MessageBuilder&& builder );

        std::string getCurrentTestName() const;

        const AssertionResult* getLastResult() const;

        void exceptionEarlyReported();

        void handleFatalErrorCondition( StringRef message );

        bool lastAssertionPassed();

        void assertionPassed();

    public:
        // !TBD We need to do this another way!
        bool aborting() const;

    private:

        void runCurrentTest( std::string& redirectedCout, std::string& redirectedCerr );
        void invokeActiveTestCase();

        void resetAssertionInfo();
        bool testForMissingAssertions( Counts& assertions );

        void assertionEnded( AssertionResult const& result );
        void reportExpr
                (   AssertionInfo const &info,
                    ResultWas::OfType resultType,
                    ITransientExpression const *expr,
                    bool negated );

        void populateReaction( AssertionReaction& reaction );

    private:

        void handleUnfinishedSections();

        TestRunInfo m_runInfo;
        TestCaseHandle const* m_activeTestCase = nullptr;
        ITracker* m_testCaseTracker = nullptr;

        IConfig const* m_config;
        Totals m_totals;
        AssertionInfo m_lastAssertionInfo;
        bool m_lastAssertionPassed = false;
        bool m_shouldReportUnexpected = true;
        bool m_includeSuccessfulResults;
    };

    void seedRng(IConfig const& config);
    unsigned int rngSeed();

    RunContext& getResultCapture();

} // end namespace Catch

#endif // CATCH_RUN_CONTEXT_HPP_INCLUDED
