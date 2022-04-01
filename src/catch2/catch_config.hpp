
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_CONFIG_HPP_INCLUDED
#define CATCH_CONFIG_HPP_INCLUDED

#include <catch2/catch_test_spec.hpp>
#include "catch2/catch_user_config.hpp"
#include <catch2/interfaces/catch_interfaces_config.hpp>
#include <catch2/internal/catch_unique_ptr.hpp>
#include <catch2/internal/catch_optional.hpp>
#include <catch2/internal/catch_random_seed_generation.hpp>

#include <iosfwd>
#include <vector>
#include <string>

namespace Catch {

    class IStream;

    struct ConfigData {
        struct ReporterAndFile {
            std::string reporterName;

            // If none, the output goes to the default output.
            Optional<std::string> outputFileName;

            friend bool operator==(ReporterAndFile const& lhs, ReporterAndFile const& rhs) {
                return lhs.reporterName == rhs.reporterName && lhs.outputFileName == rhs.outputFileName;
            }
            friend std::ostream& operator<<(std::ostream &os, ReporterAndFile const& reporter);
        };

        bool listTests = false;
        bool listTags = false;
        bool listReporters = false;

        bool showSuccessfulTests = false;
        bool shouldDebugBreak = false;
        bool noThrow = false;
        bool showHelp = false;
        bool showInvisibles = false;
        bool filenamesAsTags = false;
        bool libIdentify = false;
        bool allowZeroTests = false;

        int abortAfter = -1;
        uint32_t rngSeed = generateRandomSeed(GenerateFrom::Default);

        unsigned int shardCount = 1;
        unsigned int shardIndex = 0;

        bool benchmarkNoAnalysis = false;
        unsigned int benchmarkSamples = 100;
        double benchmarkConfidenceInterval = 0.95;
        unsigned int benchmarkResamples = 100000;
        std::chrono::milliseconds::rep benchmarkWarmupTime = 100;

        Verbosity verbosity = Verbosity::Normal;
        WarnAbout::What warnings = WarnAbout::Nothing;
        ShowDurations showDurations = ShowDurations::DefaultForReporter;
        double minDuration = -1;
        TestRunOrder runOrder = TestRunOrder::Declared;
        ColourMode colourMode = ColourMode::PlatformDefault;
        WaitForKeypress::When waitForKeypress = WaitForKeypress::Never;

        std::string defaultOutputFilename;
        std::string name;
        std::string processName;
        std::vector<ReporterAndFile> reporterSpecifications;

        std::vector<std::string> testsOrTags;
        std::vector<std::string> sectionsToRun;
    };


    class Config : public IConfig {
    public:

        Config() = default;
        Config( ConfigData const& data );
        ~Config() override; // = default in the cpp file

        bool listTests() const;
        bool listTags() const;
        bool listReporters() const;

        std::vector<ConfigData::ReporterAndFile> const& getReportersAndOutputFiles() const;
        IStream const* getReporterOutputStream(std::size_t reporterIdx) const;

        std::vector<std::string> const& getTestsOrTags() const override;
        std::vector<std::string> const& getSectionsToRun() const override;

        TestSpec const& testSpec() const override;
        bool hasTestFilters() const override;

        bool showHelp() const;

        // IConfig interface
        bool allowThrows() const override;
        StringRef name() const override;
        bool includeSuccessfulResults() const override;
        bool warnAboutMissingAssertions() const override;
        bool warnAboutUnmatchedTestSpecs() const override;
        bool zeroTestsCountAsSuccess() const override;
        ShowDurations showDurations() const override;
        double minDuration() const override;
        TestRunOrder runOrder() const override;
        uint32_t rngSeed() const override;
        unsigned int shardCount() const override;
        unsigned int shardIndex() const override;
        ColourMode colourMode() const override;
        bool shouldDebugBreak() const override;
        int abortAfter() const override;
        bool showInvisibles() const override;
        Verbosity verbosity() const override;
        bool benchmarkNoAnalysis() const override;
        unsigned int benchmarkSamples() const override;
        double benchmarkConfidenceInterval() const override;
        unsigned int benchmarkResamples() const override;
        std::chrono::milliseconds benchmarkWarmupTime() const override;

    private:
        Detail::unique_ptr<IStream const> openStream(std::string const& outputFileName);
        ConfigData m_data;

        std::vector<Detail::unique_ptr<IStream const>> m_reporterStreams;
        TestSpec m_testSpec;
        bool m_hasTestFilters = false;
    };
} // end namespace Catch

#endif // CATCH_CONFIG_HPP_INCLUDED
