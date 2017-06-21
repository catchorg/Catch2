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
#include <ctime>

namespace Catch {

    inline clara::Parser makeCommandLineParser( ConfigData& config ) {

        using namespace clara;

        auto const setWarning = [&]( std::string const& warning ) {
                if( warning != "NoAssertions" )
                    return ParserResult::runtimeError( "Unrecognised warning: '" + warning + "'" );
                config.warnings = static_cast<WarnAbout::What>( config.warnings | WarnAbout::NoAssertions );
                return ParserResult::ok( ParseResultType::Matched );
            };
        auto const loadTestNamesFromFile = [&]( std::string const& filename ) {
                std::ifstream f( filename.c_str() );
                if( !f.is_open() )
                    return ParserResult::runtimeError( "Unable to load input file: '" + filename + "'" );

                std::string line;
                while( std::getline( f, line ) ) {
                    line = trim(line);
                    if( !line.empty() && !startsWith( line, '#' ) ) {
                        if( !startsWith( line, '"' ) )
                            line = '"' + line + '"';
                        config.testsOrTags.push_back( line + ',' );
                    }
                }
                return ParserResult::ok( ParseResultType::Matched );
            };
        auto const setTestOrder = [&]( std::string const& order ) {
                if( startsWith( "declared", order ) )
                    config.runOrder = RunTests::InDeclarationOrder;
                else if( startsWith( "lexical", order ) )
                    config.runOrder = RunTests::InLexicographicalOrder;
                else if( startsWith( "random", order ) )
                    config.runOrder = RunTests::InRandomOrder;
                else
                    return clara::ParserResult::runtimeError( "Unrecognised ordering: '" + order + "'" );
                return ParserResult::ok( ParseResultType::Matched );
            };
        auto const setRngSeed = [&]( std::string const& seed ) {
                if( seed != "time" )
                    return clara::detail::convertInto( seed, config.rngSeed );
                config.rngSeed = static_cast<unsigned int>( std::time(0) );
                return ParserResult::ok( ParseResultType::Matched );
            };
        auto const setColourUsage = [&]( std::string const& useColour ) {
                    auto mode = toLower( useColour );

                    if( mode == "yes" )
                        config.useColour = UseColour::Yes;
                    else if( mode == "no" )
                        config.useColour = UseColour::No;
                    else if( mode == "auto" )
                        config.useColour = UseColour::Auto;
                    else
                        return ParserResult::runtimeError( "colour mode must be one of: auto, yes or no. '" + useColour + "' not recognised" );
                return ParserResult::ok( ParseResultType::Matched );
            };

        auto cli
            = ExeName( config.processName )
            + Help( config.showHelp );

        cli += Opt( config.listTests )
                ["-l"]["--list-tests"]
                ( "list all/matching test cases" );
        cli += Opt( config.listTags )
                ["-t"]["--list-tags"]
                ( "list all/matching tags" );
        cli += Opt( config.showSuccessfulTests )
                ["-s"]["--success"]
                ( "include successful tests in output" );
        cli += Opt( config.shouldDebugBreak )
                ["-b"]["--break"]
                ( "break into debugger on failure" );
        cli += Opt( config.noThrow )
                ["-e"]["--nothrow"]
                ( "skip exception tests" );
        cli += Opt( config.showInvisibles )
                ["-i"]["--invisibles"]
                ( "show invisibles (tabs, newlines)" );
        cli += Opt( config.outputFilename, "filename" )
                ["-o"]["--out"]
                ( "output filename" );
        cli += Opt( config.reporterNames, "name" )
                ["-r"]["--reporter"]
                ( "reporter to use (defaults to console)" );
        cli += Opt( config.name, "name" )
                ["-n"]["--name"]
                ( "suite name" );
        cli += Opt( [&]( bool ){ config.abortAfter = 1; } )
                ["-a"]["--abort"]
                ( "abort at first failure" );
        cli += Opt( [&]( int x ){ config.abortAfter = x; }, "no. failures" )
                ["-x"]["--abortx"]
                ( "abort after x failures" );
        cli += Opt( setWarning, "warning name" )
                ["-w"]["--warn"]
                ( "enable warnings" );
        cli += Opt( [&]( bool ) { config.showDurations = ShowDurations::Always; } )
                ["-d"]["--durations"]
                ( "show test durations" );
        cli += Opt( loadTestNamesFromFile, "filename" )
                ["-f"]["--input-file"]
                ( "load test names to run from a file" );
        cli += Opt( config.filenamesAsTags )
                ["-#"]["--filenames-as-tags"]
                ( "adds a tag for the filename" );
        cli += Opt( config.sectionsToRun, "section name" )
                ["-c"]["--section"]
                ( "specify section to run" );
        cli += Opt( config.listTestNamesOnly )
                ["--list-test-names-only"]
                ( "list all/matching test cases names only" );
        cli += Opt( config.listReporters )
                ["--list-reporters"]
                ( "list all reporters" );
        cli += Opt( setTestOrder, "decl|lex|rand" )
                ["--order"]
                ( "test case order (defaults to decl)" );
        cli += Opt( setRngSeed, "'time'|number" )
                ["--rng-seed"]
                ( "set a specific seed for random numbers" );
        cli += Opt( setColourUsage, "yes|no" )
                ["--use-colour"]
                ( "should output be colourised" );
        cli += Arg( config.testsOrTags, "test name|pattern|tags" )
                ( "which test or tests to use" );

        return cli;
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_COMMANDLINE_HPP_INCLUDED
