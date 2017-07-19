/*
 *  Created by Phil on 08/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED

#include "catch_test_spec_parser.hpp"
#include "catch_context.h"
#include "catch_interfaces_config.h"
#include "catch_stream.h"

#include <memory>
#include <vector>
#include <string>

#ifndef CATCH_CONFIG_CONSOLE_WIDTH
#define CATCH_CONFIG_CONSOLE_WIDTH 80
#endif

namespace Catch {

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

        int abortAfter = -1;
        unsigned int rngSeed = 0;

        Verbosity verbosity = Verbosity::Normal;
        WarnAbout::What warnings = WarnAbout::Nothing;
        ShowDurations::OrNot showDurations = ShowDurations::DefaultForReporter;
        RunTests::InWhatOrder runOrder = RunTests::InDeclarationOrder;
        UseColour::YesOrNo useColour = UseColour::Auto;

        std::string outputFilename;
        std::string name;
        std::string processName;

        std::vector<std::string> reporterNames;
        std::vector<std::string> testsOrTags;
        std::vector<std::string> sectionsToRun;
    };


    class Config : public IConfig {
        virtual void dummy();
    public:

        Config() = default;
        Config( ConfigData const& data );
        virtual ~Config() = default;

        std::string const& getFilename() const;

        bool listTests() const;
        bool listTestNamesOnly() const;
        bool listTags() const;
        bool listReporters() const;

        Verbosity verbosity() const;

        std::string getProcessName() const;

        std::vector<std::string> const& getReporterNames() const;
        std::vector<std::string> const& getSectionsToRun() const override;

        virtual TestSpec const& testSpec() const override;

        bool showHelp() const;

        // IConfig interface
        virtual bool allowThrows() const override;
        virtual std::ostream& stream() const override;
        virtual std::string name() const override;
        virtual bool includeSuccessfulResults() const override;
        virtual bool warnAboutMissingAssertions() const override;
        virtual ShowDurations::OrNot showDurations() const override;
        virtual RunTests::InWhatOrder runOrder() const override;
        virtual unsigned int rngSeed() const override;
        virtual UseColour::YesOrNo useColour() const override;
        virtual bool shouldDebugBreak() const override;
        virtual int abortAfter() const override;
        virtual bool showInvisibles() const override;

    private:

        IStream const* openStream();
        ConfigData m_data;

        std::unique_ptr<IStream const> m_stream;
        TestSpec m_testSpec;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED
