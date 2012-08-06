/*
 *  Created by Phil on 31/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RUNNER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_RUNNER_HPP_INCLUDED

#include "internal/catch_context_impl.hpp"

#include "internal/catch_commandline.hpp"
#include "internal/catch_list.hpp"
#include "reporters/catch_reporter_basic.hpp"
#include "reporters/catch_reporter_xml.hpp"
#include "reporters/catch_reporter_junit.hpp"

#include <fstream>
#include <stdlib.h>
#include <limits>

namespace Catch {

    INTERNAL_CATCH_REGISTER_REPORTER( "basic", BasicReporter )
    INTERNAL_CATCH_REGISTER_REPORTER( "xml", XmlReporter )
    INTERNAL_CATCH_REGISTER_REPORTER( "junit", JunitReporter )
    
    inline int Main( Config& config ) {
    
        std::string reporterName = config.data().reporter.empty()
            ? "basic"
            : config.data().reporter;
            
        ReporterConfig reporterConfig( config.getName(), config.stream(), config.includeSuccessfulResults() );
        
        Ptr<IReporter> reporter = getCurrentContext().getReporterRegistry().create( reporterName, reporterConfig );

        if( !reporter )
        {
            std::cerr << "No reporter registered with name: '" << reporterName << "'" << std::endl;
            return (std::numeric_limits<int>::max)();
        }
        
        if( !config.data().stream.empty() ) {
            if( config.data().stream[0] == '%' )
                config.useStream( config.data().stream.substr( 1 ) );
            else
                config.setFilename( config.data().stream );
        }

        // Handle list request
        if( config.listWhat() != List::None )
            return List( config );
        
        // Open output file, if specified
        std::ofstream ofs;
        if( !config.getFilename().empty() ) {
            ofs.open( config.getFilename().c_str() );
            if( ofs.fail() ) {
                std::cerr << "Unable to open file: '" << config.getFilename() << "'" << std::endl;
                return (std::numeric_limits<int>::max)();
            }
            config.setStreamBuf( ofs.rdbuf() );
        }

        int result = 0;
        
        // Scope here for the Runner so it can use the context before it is cleaned-up
        {
            Runner runner( config, reporter );

            // Run test specs specified on the command line - or default to all
            if( !config.testsSpecified() ) {
                runner.runAll();
            }
            else {
                // !TBD We should get all the testcases upfront, report any missing,
                // then just run them
                std::vector<std::string>::const_iterator it = config.getTestSpecs().begin();
                std::vector<std::string>::const_iterator itEnd = config.getTestSpecs().end();
                for(; it != itEnd; ++it ) {
                    if( runner.runMatching( *it ) == 0 ) {
                        std::cerr << "\n[No test cases matched with: " << *it << "]" << std::endl;
                    }
                }
            }
            result = static_cast<int>( runner.getTotals().assertions.failed );
        }
        Catch::Context::cleanUp();
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
                Catch::Context::cleanUp();        
                return 0;
            }
        
            parseIntoConfig( parser, config.data() );            
        }
        catch( std::exception& ex ) {
            std::cerr << ex.what() << "\n\nUsage: ...\n\n";
            showUsage( std::cerr );
            Catch::Context::cleanUp();
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
