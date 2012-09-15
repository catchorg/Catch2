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

#include <fstream>
#include <stdlib.h>
#include <limits>

namespace Catch {

    class Runner2 { // This will become Runner when Runner becomes Context

    public:
        Runner2( Config& configWrapper )
        :   m_configWrapper( configWrapper ),
            m_config( configWrapper.data() )
        {
            resolveStream();
            makeReporter();
        }

        Totals runTests() {

            std::vector<TestCaseFilters> filterGroups = m_config.filters;
            if( filterGroups.empty() ) {
                TestCaseFilters filterGroup( "" );
                filterGroups.push_back( filterGroup );
            }

            Runner context( m_configWrapper, m_reporter ); // This Runner will be renamed Context
            Totals totals;

            std::vector<TestCaseFilters>::const_iterator it = filterGroups.begin();
            std::vector<TestCaseFilters>::const_iterator itEnd = filterGroups.end();
            for(; it != itEnd && !context.aborting(); ++it ) {
                m_reporter->StartGroup( it->getName() );
                totals += runTestsForGroup( context, *it );
                if( context.aborting() )
                    m_reporter->Aborted();
                m_reporter->EndGroup( it->getName(), totals );
            }
            return totals;
        }

        Totals runTestsForGroup( Runner& context, const TestCaseFilters& filterGroup ) {
            Totals totals;
            std::vector<TestCaseInfo>::const_iterator it = getRegistryHub().getTestCaseRegistry().getAllTests().begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = getRegistryHub().getTestCaseRegistry().getAllTests().end();
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
            if( testsRunForGroup == 0 )
                std::cerr << "\n[No test cases matched with: " << filterGroup.getName() << "]" << std::endl;
            return totals;
            
        }

    private:
        void resolveStream() {
            if( !m_config.stream.empty() ) {
                if( m_config.stream[0] == '%' )
                    m_configWrapper.useStream( m_config.stream.substr( 1 ) );
                else
                    m_configWrapper.setFilename( m_config.stream );
            }
            // Open output file, if specified
            if( !m_config.outputFilename.empty() ) {
                m_ofs.open( m_config.outputFilename.c_str() );
                if( m_ofs.fail() ) {
                    std::ostringstream oss;
                    oss << "Unable to open file: '" << m_config.outputFilename << "'";
                    throw std::domain_error( oss.str() );
                }
                m_configWrapper.setStreamBuf( m_ofs.rdbuf() );
            }
        }
        void makeReporter() {
            std::string reporterName = m_config.reporter.empty()
            ? "basic"
            : m_config.reporter;

            ReporterConfig reporterConfig( m_config.name, m_configWrapper.stream(), m_config.includeWhichResults == Include::SuccessfulResults, m_config );

            m_reporter = getRegistryHub().getReporterRegistry().create( reporterName, reporterConfig );
            if( !m_reporter ) {
                std::ostringstream oss;
                oss << "No reporter registered with name: '" << reporterName << "'";
                throw std::domain_error( oss.str() );
            }
        }
        
    private:
        Config& m_configWrapper;
        const ConfigData& m_config;
        std::ofstream m_ofs;
        Ptr<IReporter> m_reporter;
        std::set<TestCaseInfo> m_testsAlreadyRun;
    };

    inline int Main( Config& configWrapper ) {
        int result = 0;
        try
        {
            Runner2 runner( configWrapper );

            const ConfigData& config = configWrapper.data();

            // Handle list request
            if( config.listSpec != List::None ) {
                List( config );
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

    inline void addIndent( std::ostream& os, std::size_t indent ) {
        while( indent-- > 0 )
            os << ' ';
    }

    inline void recursivelyWrapLine( std::ostream& os, std::string paragraph, std::size_t columns, std::size_t indent ) {
        std::size_t width = columns-indent;
        std::size_t tab = 0;
        std::size_t wrapPoint = width;
        for( std::size_t pos = 0; pos < paragraph.size(); ++pos ) {
            if( pos == width ) {
                addIndent( os, indent );
                os << paragraph.substr( 0, wrapPoint ) << "\n";
                return recursivelyWrapLine( os, paragraph.substr( wrapPoint+1 ), columns, indent+tab );
            }
            if( paragraph[pos] == '\t' ) {
                    tab = pos;
                    paragraph = paragraph.substr( 0, tab ) + paragraph.substr( tab+1 );
                    pos--;
            }
            else if( paragraph[pos] == ' ' ) {
                wrapPoint = pos;
            }
        }
        addIndent( os, indent );
        os << paragraph << "\n";
    }

    inline std::string addLineBreaks( const std::string& str, std::size_t columns, std::size_t indent = 0 ) {
        std::ostringstream oss;
        std::string::size_type pos = 0;
        std::string::size_type newline = str.find_first_of( '\n' );
        while( newline != std::string::npos ) {
            std::string paragraph = str.substr( pos, newline-pos );
            recursivelyWrapLine( oss, paragraph, columns, indent );
            pos = newline+1;
            newline = str.find_first_of( '\n', pos );
        }
        if( pos != str.size() )
            recursivelyWrapLine( oss, str.substr( pos, str.size()-pos ), columns, indent );

        return oss.str();
    }

    inline void showHelp( const CommandParser& parser ) {
        std::string exeName = parser.exeName();
        std::string::size_type pos = exeName.find_last_of( "/\\" );
        if( pos != std::string::npos ) {
            exeName = exeName.substr( pos+1 );
        }

        AllOptions options;
        Options::HelpOptionParser helpOpt;
        bool displayedSpecificOption = false;
        for( AllOptions::const_iterator it = options.begin(); it != options.end(); ++it ) {
            OptionParser& opt = **it;
            if( opt.find( parser ) && opt.optionNames() != helpOpt.optionNames() ) {
                displayedSpecificOption = true;
                std::cout   << "\n" << opt.optionNames() << " " << opt.argsSynopsis() << "\n\n"
                            << opt.optionSummary() << "\n\n"

                << addLineBreaks( opt.optionDescription(), 80, 2 ) << "\n" << std::endl;
            }
        }

        if( !displayedSpecificOption ) {
            std::cout << exeName << " is a CATCH host application. Options are as follows:\n\n";
            showUsage( std::cout );
        }
    }
    
    inline int Main( int argc, char* const argv[], Config& config ) {

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

            options.parseIntoConfig( parser, config.data() );
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
