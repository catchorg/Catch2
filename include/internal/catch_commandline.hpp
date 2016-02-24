/*
 *  Created by Phil on 02/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_COMMANDLINE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_COMMANDLINE_HPP_INCLUDED

#include "catch_config.hpp"
#include "catch_common.h"
#include "catch_clara.h"

#include <fstream>

namespace Catch {

    inline void abortAfterFirst( ConfigData& config ) { config.abortAfter = 1; }
    inline void abortAfterX( ConfigData& config, int x ) {
        if( x < 1 )
            throw std::runtime_error( "Value after -x or --abortAfter must be greater than zero" );
        config.abortAfter = x;
    }
    inline void addTestOrTags( ConfigData& config, std::string const& _testSpec ) { config.testsOrTags.push_back( _testSpec ); }
    inline void addReporterName( ConfigData& config, std::string const& _reporterName ) { config.reporterNames.push_back( _reporterName ); }

    inline void addWarning( ConfigData& config, std::string const& _warning ) {
        if( _warning == "NoAssertions" )
            config.warnings = static_cast<WarnAbout::What>( config.warnings | WarnAbout::NoAssertions );
        else
            throw std::runtime_error( "Unrecognised warning: '" + _warning + "'" );
    }
    inline void setOrder( ConfigData& config, std::string const& order ) {
        if( startsWith( "declared", order ) )
            config.runOrder = RunTests::InDeclarationOrder;
        else if( startsWith( "lexical", order ) )
            config.runOrder = RunTests::InLexicographicalOrder;
        else if( startsWith( "random", order ) )
            config.runOrder = RunTests::InRandomOrder;
        else
            throw std::runtime_error( "Unrecognised ordering: '" + order + "'" );
    }
    inline void setRngSeed( ConfigData& config, std::string const& seed ) {
        if( seed == "time" ) {
            config.rngSeed = static_cast<unsigned int>( std::time(0) );
        }
        else {
            std::stringstream ss;
            ss << seed;
            ss >> config.rngSeed;
            if( ss.fail() )
                throw std::runtime_error( "Argment to --rng-seed should be the word 'time' or a number" );
        }
    }
    inline void setVerbosity( ConfigData& config, int level ) {
        // !TBD: accept strings?
        config.verbosity = static_cast<Verbosity::Level>( level );
    }
    inline void setShowDurations( ConfigData& config, bool _showDurations ) {
        config.showDurations = _showDurations
            ? ShowDurations::Always
            : ShowDurations::Never;
    }
    inline void setUseColour( ConfigData& config, std::string const& value ) {
        std::string mode = toLower( value );
        
        if( mode == "yes" )
            config.useColour = UseColour::Yes;
        else if( mode == "no" )
            config.useColour = UseColour::No;
        else if( mode == "auto" )
            config.useColour = UseColour::Auto;
        else
            throw std::runtime_error( "colour mode must be one of: auto, yes or no" );
    }
    inline void forceColour( ConfigData& config ) {
        config.useColour = UseColour::Yes;
    }
    inline void loadTestNamesFromFile( ConfigData& config, std::string const& _filename ) {
        std::ifstream f( _filename.c_str() );
        if( !f.is_open() )
            throw std::domain_error( "Unable to load input file: " + _filename );

        std::string line;
        while( std::getline( f, line ) ) {
            line = trim(line);
            if( !line.empty() && !startsWith( line, "#" ) )
                addTestOrTags( config, "\"" + line + "\"," );
        }
    }

    inline Clara::CommandLine<ConfigData> makeCommandLineParser() {

        using namespace Clara;
        CommandLine<ConfigData> cli;

        cli.bindProcessName( &ConfigData::processName );

        cli["-?"]["-h"]["--help"]
            .describe( "display usage information" )
            .bind( &ConfigData::showHelp );

        cli["-l"]["--list-tests"]
            .describe( "list all/matching test cases" )
            .bind( &ConfigData::listTests );

        cli["-t"]["--list-tags"]
            .describe( "list all/matching tags" )
            .bind( &ConfigData::listTags );

        cli["-s"]["--success"]
            .describe( "include successful tests in output" )
            .bind( &ConfigData::showSuccessfulTests );

        cli["-b"]["--break"]
            .describe( "break into debugger on failure" )
            .bind( &ConfigData::shouldDebugBreak );

        cli["-e"]["--nothrow"]
            .describe( "skip exception tests" )
            .bind( &ConfigData::noThrow );

        cli["-i"]["--invisibles"]
            .describe( "show invisibles (tabs, newlines)" )
            .bind( &ConfigData::showInvisibles );

        cli["-o"]["--out"]
            .describe( "output filename" )
            .bind( &ConfigData::outputFilename, "filename" );

        cli["-r"]["--reporter"]
//            .placeholder( "name[:filename]" )
            .describe( "reporter to use (defaults to console)" )
            .bind( &addReporterName, "name" );

        cli["-n"]["--name"]
            .describe( "suite name" )
            .bind( &ConfigData::name, "name" );

        cli["-a"]["--abort"]
            .describe( "abort at first failure" )
            .bind( &abortAfterFirst );

        cli["-x"]["--abortx"]
            .describe( "abort after x failures" )
            .bind( &abortAfterX, "no. failures" );

        cli["-w"]["--warn"]
            .describe( "enable warnings" )
            .bind( &addWarning, "warning name" );

// - needs updating if reinstated
//        cli.into( &setVerbosity )
//            .describe( "level of verbosity (0=no output)" )
//            .shortOpt( "v")
//            .longOpt( "verbosity" )
//            .placeholder( "level" );

        cli[_]
            .describe( "which test or tests to use" )
            .bind( &addTestOrTags, "test name, pattern or tags" );

        cli["-d"]["--durations"]
            .describe( "show test durations" )
            .bind( &setShowDurations, "yes|no" );

        cli["-f"]["--input-file"]
            .describe( "load test names to run from a file" )
            .bind( &loadTestNamesFromFile, "filename" );

        cli["-#"]["--filenames-as-tags"]
            .describe( "adds a tag for the filename" )
            .bind( &ConfigData::filenamesAsTags );

        // Less common commands which don't have a short form
        cli["--list-test-names-only"]
            .describe( "list all/matching test cases names only" )
            .bind( &ConfigData::listTestNamesOnly );

        cli["--list-reporters"]
            .describe( "list all reporters" )
            .bind( &ConfigData::listReporters );

        cli["--order"]
            .describe( "test case order (defaults to decl)" )
            .bind( &setOrder, "decl|lex|rand" );

        cli["--rng-seed"]
            .describe( "set a specific seed for random numbers" )
            .bind( &setRngSeed, "'time'|number" );

        cli["--force-colour"]
            .describe( "force colourised output (deprecated)" )
            .bind( &forceColour );
        
        cli["--use-colour"]
            .describe( "should output be colourised" )
            .bind( &setUseColour, "yes|no" );

        return cli;
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_COMMANDLINE_HPP_INCLUDED
