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
#include "clara.h"

namespace Catch {

    inline void abortAfterFirst( ConfigData& config ) { config.abortAfter = 1; }
    inline void abortAfterX( ConfigData& config, int x ) {
        if( x < 1 )
            throw std::runtime_error( "Value after -x or --abortAfter must be greater than zero" );
        config.abortAfter = x;
    }
    inline void addTestOrTags( ConfigData& config, std::string const& _testSpec ) { config.testsOrTags.push_back( _testSpec ); }

    inline void addWarning( ConfigData& config, std::string const& _warning ) {
        if( _warning == "NoAssertions" )
            config.warnings = (ConfigData::WarnAbout::What)( config.warnings | ConfigData::WarnAbout::NoAssertions );
        else
            throw std::runtime_error( "Unrecognised warning: '" + _warning + "'" );

    }
    inline void setVerbosity( ConfigData& config, int level ) {
        // !TBD: accept strings?
        config.verbosity = (ConfigData::Verbosity::Level)level;
    }

    inline Clara::CommandLine<ConfigData> makeCommandLineParser() {

        Clara::CommandLine<ConfigData> cli;

        cli.bindProcessName( &ConfigData::processName );

        cli.bind( &ConfigData::showHelp )
            .describe( "display usage information" )
            .shortOpt( "?")
            .shortOpt( "h")
            .longOpt( "help" );

        cli.bind( &ConfigData::listTests )
            .describe( "list all (or matching) test cases" )
            .shortOpt( "l")
            .longOpt( "list-tests" );

        cli.bind( &ConfigData::listTags )
            .describe( "list all (or matching) tags" )
            .shortOpt( "t")
            .longOpt( "list-tags" );

        cli.bind( &ConfigData::listReporters )
            .describe( "list all reporters" )
            .longOpt( "list-reporters" );

        cli.bind( &ConfigData::showSuccessfulTests )
            .describe( "include successful tests in output" )
            .shortOpt( "s")
            .longOpt( "success" );

        cli.bind( &ConfigData::shouldDebugBreak )
            .describe( "break into debugger on failure" )
            .shortOpt( "b")
            .longOpt( "break" );

        cli.bind( &ConfigData::noThrow )
            .describe( "skip exception tests" )
            .shortOpt( "e")
            .longOpt( "nothrow" );

        cli.bind( &ConfigData::outputFilename )
            .describe( "output filename" )
            .shortOpt( "o")
            .longOpt( "out" )
            .argName( "filename" );

        cli.bind( &ConfigData::reporterName )
            .describe( "reporter to use - defaults to console" )
            .shortOpt( "r")
            .longOpt( "reporter" )
//            .argName( "name[:filename]" );
            .argName( "name" );

        cli.bind( &ConfigData::name )
            .describe( "suite name" )
            .shortOpt( "n")
            .longOpt( "name" )
            .argName( "name" );

        cli.bind( &abortAfterFirst )
            .describe( "abort at first failure" )
            .shortOpt( "a")
            .longOpt( "abort" );

        cli.bind( &abortAfterX )
            .describe( "abort after x failures" )
            .shortOpt( "x")
            .longOpt( "abortx" )
            .argName( "number of failures" );

        cli.bind( &addWarning )
            .describe( "enable warnings" )
            .shortOpt( "w")
            .longOpt( "warn" )
            .argName( "warning name" );

//        cli.bind( &setVerbosity )
//            .describe( "level of verbosity (0=no output)" )
//            .shortOpt( "v")
//            .longOpt( "verbosity" )
//            .argName( "level" );

        cli.bind( &addTestOrTags )
            .describe( "which test or tests to use" )
            .argName( "test name, pattern or tags" );

        return cli;
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_COMMANDLINE_HPP_INCLUDED
