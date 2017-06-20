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
#include <stdexcept>

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

        Config()
        {}

        Config( ConfigData const& data )
        :   m_data( data ),
            m_stream( openStream() )
        {
            if( !data.testsOrTags.empty() ) {
                TestSpecParser parser( ITagAliasRegistry::get() );
                for( auto const& testOrTags : data.testsOrTags )
                    parser.parse( testOrTags );
                m_testSpec = parser.testSpec();
            }
        }

        virtual ~Config() {}

        std::string const& getFilename() const {
            return m_data.outputFilename ;
        }

        bool listTests() const          { return m_data.listTests; }
        bool listTestNamesOnly() const  { return m_data.listTestNamesOnly; }
        bool listTags() const           { return m_data.listTags; }
        bool listReporters() const      { return m_data.listReporters; }

        Verbosity verbosity() const     { return m_data.verbosity; }

        std::string getProcessName() const { return m_data.processName; }

        std::vector<std::string> const& getReporterNames() const { return m_data.reporterNames; }
        std::vector<std::string> const& getSectionsToRun() const override { return m_data.sectionsToRun; }

        virtual TestSpec const& testSpec() const override { return m_testSpec; }

        bool showHelp() const { return m_data.showHelp; }

        // IConfig interface
        virtual bool allowThrows() const override                   { return !m_data.noThrow; }
        virtual std::ostream& stream() const override               { return m_stream->stream(); }
        virtual std::string name() const override                   { return m_data.name.empty() ? m_data.processName : m_data.name; }
        virtual bool includeSuccessfulResults() const override      { return m_data.showSuccessfulTests; }
        virtual bool warnAboutMissingAssertions() const override    { return m_data.warnings & WarnAbout::NoAssertions; }
        virtual ShowDurations::OrNot showDurations() const override { return m_data.showDurations; }
        virtual RunTests::InWhatOrder runOrder() const override     { return m_data.runOrder; }
        virtual unsigned int rngSeed() const override               { return m_data.rngSeed; }
        virtual UseColour::YesOrNo useColour() const override       { return m_data.useColour; }
        virtual bool shouldDebugBreak() const override              { return m_data.shouldDebugBreak; }
        virtual int abortAfter() const override                     { return m_data.abortAfter; }
        virtual bool showInvisibles() const override                { return m_data.showInvisibles; }

    private:

        IStream const* openStream() {
            if( m_data.outputFilename.empty() )
                return new CoutStream();
            else if( m_data.outputFilename[0] == '%' ) {
                if( m_data.outputFilename == "%debug" )
                    return new DebugOutStream();
                else
                    CATCH_ERROR( "Unrecognised stream: '" << m_data.outputFilename << "'" );
            }
            else
                return new FileStream( m_data.outputFilename );
        }
        ConfigData m_data;

        std::unique_ptr<IStream const> m_stream;
        TestSpec m_testSpec;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED
