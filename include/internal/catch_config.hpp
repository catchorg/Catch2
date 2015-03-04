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
#include <iostream>
#include <ctime>

#ifndef CATCH_CONFIG_CONSOLE_WIDTH
#define CATCH_CONFIG_CONSOLE_WIDTH 80
#endif

namespace Catch {

    struct ConfigData {

        ConfigData()
        :   listTests( false ),
            listTags( false ),
            listReporters( false ),
            listTestNamesOnly( false ),
            showSuccessfulTests( false ),
            shouldDebugBreak( false ),
            noThrow( false ),
            showHelp( false ),
            showInvisibles( false ),
            forceColour( false ),
            abortAfter( -1 ),
            rngSeed( 0 ),
            verbosity( Verbosity::Normal ),
            warnings( WarnAbout::Nothing ),
            showDurations( ShowDurations::DefaultForReporter ),
            runOrder( RunTests::InDeclarationOrder )
        {}

        bool listTests;
        bool listTags;
        bool listReporters;
        bool listTestNamesOnly;

        bool showSuccessfulTests;
        bool shouldDebugBreak;
        bool noThrow;
        bool showHelp;
        bool showInvisibles;
        bool forceColour;

        int abortAfter;
        unsigned int rngSeed;

        Verbosity::Level verbosity;
        WarnAbout::What warnings;
        ShowDurations::OrNot showDurations;
        RunTests::InWhatOrder runOrder;

        std::string reporterName;
        std::string outputFilename;
        std::string name;
        std::string processName;

        std::vector<std::string> testsOrTags;
    };


    class Config : public SharedImpl<IConfig> {
    private:
        Config( Config const& other );
        Config& operator = ( Config const& other );
        virtual void dummy();
    public:

        Config()
        :   m_os( Catch::cout().rdbuf() )
        {}

        Config( ConfigData const& data )
        :   m_data( data ),
            m_os( Catch::cout().rdbuf() )
        {
            if( !data.testsOrTags.empty() ) {
                TestSpecParser parser( ITagAliasRegistry::get() );
                for( std::size_t i = 0; i < data.testsOrTags.size(); ++i )
                    parser.parse( data.testsOrTags[i] );
                m_testSpec = parser.testSpec();
            }
        }

        virtual ~Config() {
            m_os.rdbuf( Catch::cout().rdbuf() );
            m_stream.release();
        }

        void setFilename( std::string const& filename ) {
            m_data.outputFilename = filename;
        }

        std::string const& getFilename() const {
            return m_data.outputFilename ;
        }

        bool listTests() const { return m_data.listTests; }
        bool listTestNamesOnly() const { return m_data.listTestNamesOnly; }
        bool listTags() const { return m_data.listTags; }
        bool listReporters() const { return m_data.listReporters; }

        std::string getProcessName() const { return m_data.processName; }

        bool shouldDebugBreak() const { return m_data.shouldDebugBreak; }

        void setStreamBuf( std::streambuf* buf ) {
            m_os.rdbuf( buf ? buf : Catch::cout().rdbuf() );
        }

        void useStream( std::string const& streamName ) {
            Stream stream = createStream( streamName );
            setStreamBuf( stream.streamBuf );
            m_stream.release();
            m_stream = stream;
        }

        std::string getReporterName() const { return m_data.reporterName; }

        int abortAfter() const { return m_data.abortAfter; }

        TestSpec const& testSpec() const { return m_testSpec; }

        bool showHelp() const { return m_data.showHelp; }
        bool showInvisibles() const { return m_data.showInvisibles; }

        // IConfig interface
        virtual bool allowThrows() const        { return !m_data.noThrow; }
        virtual std::ostream& stream() const    { return m_os; }
        virtual std::string name() const        { return m_data.name.empty() ? m_data.processName : m_data.name; }
        virtual bool includeSuccessfulResults() const   { return m_data.showSuccessfulTests; }
        virtual bool warnAboutMissingAssertions() const { return m_data.warnings & WarnAbout::NoAssertions; }
        virtual ShowDurations::OrNot showDurations() const { return m_data.showDurations; }
        virtual RunTests::InWhatOrder runOrder() const  { return m_data.runOrder; }
        virtual unsigned int rngSeed() const    { return m_data.rngSeed; }
        virtual bool forceColour() const { return m_data.forceColour; }

    private:
        ConfigData m_data;

        Stream m_stream;
        mutable std::ostream m_os;
        TestSpec m_testSpec;
    };


} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED
