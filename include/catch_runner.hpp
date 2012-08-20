/*
 *  Created by Phil on 31/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RUNNER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_RUNNER_HPP_INCLUDED

#include "internal/catch_commandline.hpp"
#include "internal/catch_list.hpp"
#include "internal/catch_runner_impl.hpp"

#include <fstream>
#include <stdlib.h>
#include <limits>

namespace Catch {

    inline int resolveStream( std::ofstream& ofs, Config& configWrapper ) {
        const ConfigData& config = configWrapper.data();
        
        if( !config.stream.empty() ) {
            if( config.stream[0] == '%' )
                configWrapper.useStream( config.stream.substr( 1 ) );
            else
                configWrapper.setFilename( config.stream );
        }
        // Open output file, if specified
        if( !config.outputFilename.empty() ) {
            ofs.open( config.outputFilename.c_str() );
            if( ofs.fail() ) {
                std::cerr << "Unable to open file: '" << config.outputFilename << "'" << std::endl;
                return (std::numeric_limits<int>::max)();
            }
            configWrapper.setStreamBuf( ofs.rdbuf() );
        }
        return 0;
    }
    
    inline Ptr<IReporter> makeReporter( Config& configWrapper ) {
        const ConfigData& config = configWrapper.data();
        
        std::string reporterName = config.reporter.empty()
            ? "basic"
            : config.reporter;

        ReporterConfig reporterConfig( config.name, configWrapper.stream(), config.includeWhichResults == Include::SuccessfulResults );

        Ptr<IReporter> reporter = getRegistryHub().getReporterRegistry().create( reporterName, reporterConfig );
        if( !reporter )
            std::cerr << "No reporter registered with name: '" << reporterName << "'" << std::endl;
        return reporter;
    }

    inline int Main( Config& configWrapper ) {

        std::ofstream ofs;
        int result = resolveStream( ofs, configWrapper );
        if( result != 0 )
            return result;

        Ptr<IReporter> reporter = makeReporter( configWrapper );
        if( !reporter )
            return (std::numeric_limits<int>::max)();
        
        const ConfigData& config = configWrapper.data();

        // Handle list request
        if( config.listSpec != List::None )
            return List( config );
        
        // Scope here for the Runner so it can use the context before it is cleaned-up
        {
            Runner runner( configWrapper, reporter );

            Totals totals;
            // Run test specs specified on the command line - or default to all
            if( config.testSpecs.empty() ) {
                totals = runner.runAllNonHidden();
            }
            else {
                std::vector<std::string>::const_iterator it = config.testSpecs.begin();
                std::vector<std::string>::const_iterator itEnd = config.testSpecs.end();
                for(; it != itEnd; ++it ) {
                    Totals groupTotals = runner.runMatching( *it );
                    if( groupTotals.testCases.total() == 0 )
                        std::cerr << "\n[No test cases matched with: " << *it << "]" << std::endl;
                    totals += groupTotals;
                }
            }
            result = static_cast<int>( totals.assertions.failed );
        }
        Catch::cleanUp();
        return result;
    }

    inline void showUsage( std::ostream& os ) {
        os  << "\t-?, -h, --help\n"
            << "\t-l, --list <tests | reporters> [xml]\n"
            << "\t-t, --test <testspec> [<testspec>...]\n"
            << "\t-r, --reporter <reporter name>\n"
            << "\t-o, --out <file name>|<%stream name>\n"
            << "\t-s, --success\n"
            << "\t-b, --break\n"
            << "\t-n, --name <name>\n"
            << "\t-a, --abort [#]\n"
            << "\t-nt, --nothrow\n\n"
            << "For more detail usage please see: https://github.com/philsquared/Catch/wiki/Command-line" << std::endl;    
    }
    inline void showHelp( std::string exeName ) {
        std::string::size_type pos = exeName.find_last_of( "/\\" );
        if( pos != std::string::npos ) {
            exeName = exeName.substr( pos+1 );
        }

        std::cout << exeName << " is a CATCH host application. Options are as follows:\n\n";
        showUsage( std::cout );
    }
    
    inline int Main( int argc, char* const argv[], Config& config ) {

        try {
            CommandParser parser( argc, argv );
        
            if( Command cmd = parser.find( "-h", "-?", "--help" ) ) {
                if( cmd.argsCount() != 0 )
                    cmd.raiseError( "Does not accept arguments" );

                showHelp( argv[0] );
                Catch::cleanUp();        
                return 0;
            }
        
            parseIntoConfig( parser, config.data() );            
        }
        catch( std::exception& ex ) {
            std::cerr << ex.what() << "\n\nUsage: ...\n\n";
            showUsage( std::cerr );
            Catch::cleanUp();
            return (std::numeric_limits<int>::max)();
        }
                
        return Main( config );
    }
    
    inline int Main( int argc, char* const argv[] ) {
        Config config;
// !TBD: This doesn't always work, for some reason
//        if( isDebuggerActive() )
//            config.useStream( "debug" );
        return Main( argc, argv, config );
    }
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RUNNER_HPP_INCLUDED
