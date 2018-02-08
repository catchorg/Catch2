/*
 *  Created by Phil on 08/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED

#include "catch_test_spec_parser.h"
#include "catch_interfaces_config.h"

// Libstdc++ doesn't like incomplete classes for unique_ptr
#include "catch_stream.h"

#include <memory>
#include <vector>
#include <string>

#ifndef CATCH_CONFIG_CONSOLE_WIDTH
#define CATCH_CONFIG_CONSOLE_WIDTH 80
#endif

namespace Catch {

    struct IStream;

    struct ConfigData {
        bool listTests = false;
        bool listTags = false;
        bool listReporters = false;
        bool listTestNamesOnly = false;

        bool showSuccessfulTests = false;
        bool shouldDebugBreak = false;
        bool noThrow = false;
        bool showHelp = false;
        bool showInvisibles = false;
        bool filenamesAsTags = false;
        bool libIdentify = false;

        int abortAfter = -1;
        unsigned int rngSeed = 0;
        int benchmarkResolutionMultiple = 100;

        Verbosity verbosity = Verbosity::Normal;
        WarnAbout::What warnings = WarnAbout::Nothing;
        ShowDurations::OrNot showDurations = ShowDurations::DefaultForReporter;
        RunTests::InWhatOrder runOrder = RunTests::InDeclarationOrder;
        UseColour::YesOrNo useColour = UseColour::Auto;
        WaitForKeypress::When waitForKeypress = WaitForKeypress::Never;

        std::string outputFilename;
        std::string name;
        std::string processName;

        std::vector<std::string> reporterNames;
        std::vector<std::string> testsOrTags;
        std::vector<std::string> sectionsToRun;
    };


    class Config : public IConfig {
    public:

        Config() = default;
        Config( ConfigData const& data );
        virtual ~Config() = default;

        std::string const& getFilename() const;

        bool listTests() const;
        bool listTestNamesOnly() const;
        bool listTags() const;
        bool listReporters() const;

        std::string getProcessName() const;

        std::vector<std::string> const& getReporterNames() const;
        std::vector<std::string> const& getTestsOrTags() const;
        std::vector<std::string> const& getSectionsToRun() const override;

        virtual TestSpec const& testSpec() const override;
        bool hasTestFilters() const override;

        bool showHelp() const;

        // IConfig interface
        bool allowThrows() const override;
        std::ostream& stream() const override;
        std::string name() const override;
        bool includeSuccessfulResults() const override;
        bool warnAboutMissingAssertions() const override;
        bool warnAboutNoTests() const override;
        ShowDurations::OrNot showDurations() const override;
        RunTests::InWhatOrder runOrder() const override;
        unsigned int rngSeed() const override;
        int benchmarkResolutionMultiple() const override;
        UseColour::YesOrNo useColour() const override;
        bool shouldDebugBreak() const override;
        int abortAfter() const override;
        bool showInvisibles() const override;
        Verbosity verbosity() const override;

    private:

        IStream const* openStream();
        ConfigData m_data;

        std::unique_ptr<IStream const> m_stream;
        TestSpec m_testSpec;
        bool m_hasTestFilters = false;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED
