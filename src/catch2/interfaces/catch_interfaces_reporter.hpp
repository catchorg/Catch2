
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_INTERFACES_REPORTER_HPP_INCLUDED
#define CATCH_INTERFACES_REPORTER_HPP_INCLUDED

#include <catch2/catch_section_info.hpp>
#include <catch2/catch_totals.hpp>
#include <catch2/catch_assertion_result.hpp>
#include <catch2/internal/catch_message_info.hpp>
#include <catch2/internal/catch_generator_info.hpp>
#include <catch2/internal/catch_stringref.hpp>
#include <catch2/internal/catch_unique_ptr.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>
#include <catch2/benchmark/catch_estimate.hpp>
#include <catch2/benchmark/catch_outlier_classification.hpp>


#include <map>
#include <string>
#include <vector>
#include <iosfwd>

namespace Catch {

    struct ReporterDescription;
    struct ListenerDescription;
    struct TagInfo;
    struct TestCaseInfo;
    class TestCaseHandle;
    class IConfig;
    class IStream;
    enum class ColourMode : std::uint8_t;

    struct ReporterConfig {
        ReporterConfig( IConfig const* _fullConfig,
                        Detail::unique_ptr<IStream> _stream,
                        ColourMode colourMode,
                        std::map<std::string, std::string> customOptions );

        ReporterConfig( ReporterConfig&& ) = default;
        ReporterConfig& operator=( ReporterConfig&& ) = default;
        ~ReporterConfig(); // = default

        Detail::unique_ptr<IStream> takeStream() &&;
        IConfig const* fullConfig() const;
        ColourMode colourMode() const;
        std::map<std::string, std::string> const& customOptions() const;

    private:
        Detail::unique_ptr<IStream> m_stream;
        IConfig const* m_fullConfig;
        ColourMode m_colourMode;
        std::map<std::string, std::string> m_customOptions;
    };

    struct TestRunInfo {
        constexpr TestRunInfo(StringRef _name) : name(_name) {}
        StringRef name;
    };

    struct AssertionStats {
        AssertionStats( AssertionResult const& _assertionResult,
                        std::vector<MessageInfo> const& _infoMessages,
                        std::vector<GeneratorInfo> const& _generatorInfos,
                        Totals const& _totals );

        AssertionStats( AssertionStats const& )              = default;
        AssertionStats( AssertionStats && )                  = default;
        AssertionStats& operator = ( AssertionStats const& ) = delete;
        AssertionStats& operator = ( AssertionStats && )     = delete;

        AssertionResult assertionResult;
        std::vector<MessageInfo> infoMessages;
        std::vector<GeneratorInfo> generatorInfos;
        Totals totals;
    };

    struct SectionStats {
        SectionStats(   SectionInfo const& _sectionInfo,
                        Counts const& _assertions,
                        double _durationInSeconds,
                        bool _missingAssertions );

        SectionInfo sectionInfo;
        Counts assertions;
        double durationInSeconds;
        bool missingAssertions;
    };

    struct TestCaseStats {
        TestCaseStats(  TestCaseInfo const& _testInfo,
                        Totals const& _totals,
                        std::string const& _stdOut,
                        std::string const& _stdErr,
                        bool _aborting );

        TestCaseInfo const * testInfo;
        Totals totals;
        std::string stdOut;
        std::string stdErr;
        bool aborting;
    };

    struct TestRunStats {
        TestRunStats(   TestRunInfo const& _runInfo,
                        Totals const& _totals,
                        bool _aborting );

        TestRunInfo runInfo;
        Totals totals;
        bool aborting;
    };


    struct BenchmarkInfo {
        std::string name;
        double estimatedDuration;
        int iterations;
        unsigned int samples;
        unsigned int resamples;
        double clockResolution;
        double clockCost;
    };

    template <class Duration>
    struct BenchmarkStats {
        BenchmarkInfo info;

        std::vector<Duration> samples;
        Benchmark::Estimate<Duration> mean;
        Benchmark::Estimate<Duration> standardDeviation;
        Benchmark::OutlierClassification outliers;
        double outlierVariance;

        template <typename Duration2>
        operator BenchmarkStats<Duration2>() const {
            std::vector<Duration2> samples2;
            samples2.reserve(samples.size());
            for (auto const& sample : samples) {
                samples2.push_back(Duration2(sample));
            }
            return {
                info,
                CATCH_MOVE(samples2),
                mean,
                standardDeviation,
                outliers,
                outlierVariance,
            };
        }
    };

    //! By setting up its preferences, a reporter can modify Catch2's behaviour
    //! in some regards, e.g. it can request Catch2 to capture writes to
    //! stdout/stderr during test execution, and pass them to the reporter.
    struct ReporterPreferences {
        //! Catch2 should redirect writes to stdout and pass them to the
        //! reporter
        bool shouldRedirectStdOut = false;
        //! Catch2 should call `Reporter::assertionEnded` even for passing
        //! assertions
        bool shouldReportAllAssertions = false;
    };

    /**
     * The common base for all reporters and event listeners
     *
     * Implementing classes must also implement:
     *
     *     //! User-friendly description of the reporter/listener type
     *     static std::string getDescription()
     *
     * Generally shouldn't be derived from by users of Catch2 directly,
     * instead they should derive from one of the utility bases that
     * derive from this class.
     */
    class IEventListener {
    protected:
        //! Derived classes can set up their preferences here
        ReporterPreferences m_preferences;
        //! The test run's config as filled in from CLI and defaults
        IConfig const* m_config;

    public:
        IEventListener( IConfig const* config ): m_config( config ) {}

        virtual ~IEventListener(); // = default;

        // Implementing class must also provide the following static methods:
        // static std::string getDescription();

        ReporterPreferences const& getPreferences() const {
            return m_preferences;
        }

        //! Called when no test cases match provided test spec
        virtual void noMatchingTestCases( StringRef unmatchedSpec ) = 0;
        //! Called for all invalid test specs from the cli
        virtual void reportInvalidTestSpec( StringRef invalidArgument ) = 0;

        /**
         * Called once in a testing run before tests are started
         *
         * Not called if tests won't be run (e.g. only listing will happen)
         */
        virtual void testRunStarting( TestRunInfo const& testRunInfo ) = 0;

        //! Called _once_ for each TEST_CASE, no matter how many times it is entered
        virtual void testCaseStarting( TestCaseInfo const& testInfo ) = 0;
        //! Called _every time_ a TEST_CASE is entered, including repeats (due to sections)
        virtual void testCasePartialStarting( TestCaseInfo const& testInfo, uint64_t partNumber ) = 0;
        //! Called when a `SECTION` is being entered. Not called for skipped sections
        virtual void sectionStarting( SectionInfo const& sectionInfo ) = 0;

        //! Called when user-code is being probed before the actual benchmark runs
        virtual void benchmarkPreparing( StringRef benchmarkName ) = 0;
        //! Called after probe but before the user-code is being benchmarked
        virtual void benchmarkStarting( BenchmarkInfo const& benchmarkInfo ) = 0;
        //! Called with the benchmark results if benchmark successfully finishes
        virtual void benchmarkEnded( BenchmarkStats<> const& benchmarkStats ) = 0;
        //! Called if running the benchmarks fails for any reason
        virtual void benchmarkFailed( StringRef benchmarkName ) = 0;

        //! Called before assertion success/failure is evaluated
        virtual void assertionStarting( AssertionInfo const& assertionInfo ) = 0;

        //! Called after assertion was fully evaluated
        virtual void assertionEnded( AssertionStats const& assertionStats ) = 0;

        //! Called after a `SECTION` has finished running
        virtual void sectionEnded( SectionStats const& sectionStats ) = 0;
        //! Called _every time_ a TEST_CASE is entered, including repeats (due to sections)
        virtual void testCasePartialEnded(TestCaseStats const& testCaseStats, uint64_t partNumber ) = 0;
        //! Called _once_ for each TEST_CASE, no matter how many times it is entered
        virtual void testCaseEnded( TestCaseStats const& testCaseStats ) = 0;
        /**
         * Called once after all tests in a testing run are finished
         *
         * Not called if tests weren't run (e.g. only listings happened)
         */
        virtual void testRunEnded( TestRunStats const& testRunStats ) = 0;

        //! Called with test cases that are skipped due to the test run aborting
        virtual void skipTest( TestCaseInfo const& testInfo ) = 0;

        //! Called if a fatal error (signal/structured exception) occured
        virtual void fatalErrorEncountered( StringRef error ) = 0;

        //! Writes out information about provided reporters using reporter-specific format
        virtual void listReporters(std::vector<ReporterDescription> const& descriptions) = 0;
        //! Writes out the provided listeners descriptions using reporter-specific format
        virtual void listListeners(std::vector<ListenerDescription> const& descriptions) = 0;
        //! Writes out information about provided tests using reporter-specific format
        virtual void listTests(std::vector<TestCaseHandle> const& tests) = 0;
        //! Writes out information about the provided tags using reporter-specific format
        virtual void listTags(std::vector<TagInfo> const& tags) = 0;
    };
    using IEventListenerPtr = Detail::unique_ptr<IEventListener>;

} // end namespace Catch

#endif // CATCH_INTERFACES_REPORTER_HPP_INCLUDED
