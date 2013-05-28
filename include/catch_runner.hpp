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
#include "internal/catch_test_spec.h"
#include "internal/catch_version.h"
#include "internal/catch_text.h"

#include <fstream>
#include <stdlib.h>
#include <limits>

namespace Catch {

    class Runner2 { // This will become Runner when Runner becomes Context

    public:
        Runner2( ConfigData const& config )
        :   m_config( new Config( config ) )
        {
            openStream();
            makeReporter();
        }

        Totals runTests() {

            std::vector<TestCaseFilters> filterGroups = m_config->filters();
            if( filterGroups.empty() ) {
                TestCaseFilters filterGroup( "" );
                filterGroups.push_back( filterGroup );
            }

            Runner context( m_config.get(), m_reporter ); // This Runner will be renamed Context
            Totals totals;

            for( std::size_t i=0; i < filterGroups.size() && !context.aborting(); ++i ) {
                context.testGroupStarting( filterGroups[i].getName(), i, filterGroups.size() );
                totals += runTestsForGroup( context, filterGroups[i] );
                context.testGroupEnded( filterGroups[i].getName(), totals, i, filterGroups.size() );
            }
            return totals;
        }

        Totals runTestsForGroup( Runner& context, const TestCaseFilters& filterGroup ) {
            Totals totals;
            std::vector<TestCase>::const_iterator it = getRegistryHub().getTestCaseRegistry().getAllTests().begin();
            std::vector<TestCase>::const_iterator itEnd = getRegistryHub().getTestCaseRegistry().getAllTests().end();
            int testsRunForGroup = 0;
            for(; it != itEnd; ++it ) {
                if( filterGroup.shouldInclude( *it ) ) {
                    testsRunForGroup++;
                    if( m_testsAlreadyRun.find( *it ) == m_testsAlreadyRun.end() ) {

                        if( context.aborting() )
                            break;

                        totals += context.runTest( *it );
                        m_testsAlreadyRun.insert( *it );
                    }
                }
            }
            if( testsRunForGroup == 0 && !filterGroup.getName().empty() )
                m_reporter->noMatchingTestCases( filterGroup.getName() );
            return totals;
            
        }

    private:
        void openStream() {
            if( !m_config->getStreamName().empty() )
                m_config->useStream( m_config->getStreamName() );

            // Open output file, if specified
            if( !m_config->getFilename().empty() ) {
                m_ofs.open( m_config->getFilename().c_str() );
                if( m_ofs.fail() ) {
                    std::ostringstream oss;
                    oss << "Unable to open file: '" << m_config->getFilename() << "'";
                    throw std::domain_error( oss.str() );
                }
                m_config->setStreamBuf( m_ofs.rdbuf() );
            }
        }
        void makeReporter() {
            std::string reporterName = m_config->getReporterName().empty()
                ? "console"
                : m_config->getReporterName();

            m_reporter = getRegistryHub().getReporterRegistry().create( reporterName, m_config.get() );
            if( !m_reporter ) {
                std::ostringstream oss;
                oss << "No reporter registered with name: '" << reporterName << "'";
                throw std::domain_error( oss.str() );
            }
        }
        
    private:
        Ptr<Config> m_config;
        std::ofstream m_ofs;
        Ptr<IStreamingReporter> m_reporter;
        std::set<TestCase> m_testsAlreadyRun;
    };

    inline int Main( ConfigData const& configData ) {
        int result = 0;
        try
        {
            Runner2 runner( configData );

            // Handle list request
            if( configData.listSpec != List::None ) {
                list( configData );
                Catch::cleanUp();
                return 0;
            }

            result = static_cast<int>( runner.runTests().assertions.failed );

        }
        catch( std::exception& ex ) {
            std::cerr << ex.what() << std::endl;
            result = (std::numeric_limits<int>::max)();
        }

        Catch::cleanUp();
        return result;
    }

    inline void showUsage( std::ostream& os ) {
        AllOptions options;

        for( AllOptions::const_iterator it = options.begin(); it != options.end(); ++it ) {
            OptionParser& opt = **it;
            os << "  " << opt.optionNames() << " " << opt.argsSynopsis() << "\n";
        }
        os << "\nFor more detail usage please see: https://github.com/philsquared/Catch/wiki/Command-line\n" << std::endl;
    }

    inline void showHelp( const CommandParser& parser ) {
        AllOptions options;
        Options::HelpOptionParser helpOpt;
        bool displayedSpecificOption = false;
        for( AllOptions::const_iterator it = options.begin(); it != options.end(); ++it ) {
            OptionParser& opt = **it;
            if( opt.find( parser ) && opt.optionNames() != helpOpt.optionNames() ) {
                displayedSpecificOption = true;
                std::cout   << "\n" << opt.optionNames() << " " << opt.argsSynopsis() << "\n\n"
                            << opt.optionSummary() << "\n\n"
                            << Text( opt.optionDescription(), TextAttributes().setIndent( 2 ) ) << "\n" << std::endl;
            }
        }

        if( !displayedSpecificOption ) {
            std::cout   << "\nCATCH v"  << libraryVersion.majorVersion << "."
                                        << libraryVersion.minorVersion << " build "
                                        << libraryVersion.buildNumber;
            if( libraryVersion.branchName != "master" )
                std::cout << " (" << libraryVersion.branchName << " branch)";

            std::cout << "\n\n" << parser.exeName() << " is a CATCH host application. Options are as follows:\n\n";
            showUsage( std::cout );
        }
    }
    
    inline int Main( int argc, char* const argv[], ConfigData configData = ConfigData() ) {

        try {
            CommandParser parser( argc, argv );

            if( Command cmd = Options::HelpOptionParser().find( parser ) ) {
                if( cmd.argsCount() != 0 )
                    cmd.raiseError( "Does not accept arguments" );

                showHelp( parser );
                Catch::cleanUp();        
                return 0;
            }

            AllOptions options;

            options.parseIntoConfig( parser, configData );
        }
        catch( std::exception& ex ) {
            std::cerr << ex.what() << "\n\nUsage: ...\n\n";
            showUsage( std::cerr );
            Catch::cleanUp();
            return (std::numeric_limits<int>::max)();
        }
                
        return Main( configData );
    }
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RUNNER_HPP_INCLUDED
