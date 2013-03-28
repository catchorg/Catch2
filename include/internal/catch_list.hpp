/*
 *  Created by Phil on 5/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_LIST_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_LIST_HPP_INCLUDED

#include "catch_commandline.hpp"
#include "catch_line_wrap.h"

#include <limits>
#include <algorithm>

namespace Catch {
    inline bool matchesFilters( const std::vector<TestCaseFilters>& filters, const TestCase& testCase ) {
        std::vector<TestCaseFilters>::const_iterator it = filters.begin();
        std::vector<TestCaseFilters>::const_iterator itEnd = filters.end();
        for(; it != itEnd; ++it )
            if( !it->shouldInclude( testCase ) )
                return false;
        return true;
    }

    inline void listTests( const ConfigData& config ) {
        if( config.filters.empty() )
            std::cout << "All available test cases:\n";
        else
            std::cout << "Matching test cases:\n";
        std::vector<TestCase> const& allTests = getRegistryHub().getTestCaseRegistry().getAllTests();
        std::vector<TestCase>::const_iterator it = allTests.begin(), itEnd = allTests.end();

        // First pass - get max tags
        std::size_t maxTagLen = 0;
        std::size_t maxNameLen = 0;
        for(; it != itEnd; ++it ) {
            maxTagLen = (std::max)( it->getTestCaseInfo().tagsAsString.size(), maxTagLen );
            maxNameLen = (std::max)( it->getTestCaseInfo().name.size(), maxNameLen );
        }
        
        // Try to fit everything in. If not shrink tag column first, down to 30
        // then shrink name column until it all fits (strings will be wrapped within column)
        while( maxTagLen + maxNameLen > CATCH_CONFIG_CONSOLE_WIDTH-5 ) {
            if( maxTagLen > 30 )
                --maxTagLen;
            else
                --maxNameLen;
        }

        std::size_t matchedTests = 0;
        for( it = allTests.begin(); it != itEnd; ++it ) {
            if( matchesFilters( config.filters, *it ) ) {
                matchedTests++;
                // !TBD: consider listAs()
                LineWrapper nameWrapper;
                nameWrapper.setRight( maxNameLen ).setIndent( 2 ).wrap( it->getTestCaseInfo().name );

                LineWrapper tagsWrapper;
                tagsWrapper.setRight( maxTagLen ).wrap( it->getTestCaseInfo().tagsAsString );
                
                for( std::size_t i = 0; i < std::max( nameWrapper.size(), tagsWrapper.size() ); ++i ) {
                    std::string nameCol;
                    if( i < nameWrapper.size() )
                        nameCol = nameWrapper[i];
                    else
                        nameCol = "    ...";
                    std::cout << nameCol << "    " << std::string( maxNameLen - nameCol.size(), ' ' );
                    if( i < tagsWrapper.size() )
                        std::cout << tagsWrapper[i];
                    std::cout << "\n";
                }
            }
        }
        if( config.filters.empty() )
            std::cout << pluralise( matchedTests, "test case" ) << std::endl;
        else
            std::cout << pluralise( matchedTests, "matching test case" ) << std::endl;
    }

    inline void listReporters( const ConfigData& /*config*/ ) {
        std::cout << "Available reports:\n";
        IReporterRegistry::FactoryMap const& factories = getRegistryHub().getReporterRegistry().getFactories();
        IReporterRegistry::FactoryMap::const_iterator it = factories.begin(), itEnd = factories.end();
        for(; it != itEnd; ++it ) {
            // !TBD: consider listAs()
            std::cout << "\t" << it->first << "\n\t\t'" << it->second->getDescription() << "'\n";
        }
        std::cout << std::endl;
    }
    
    inline void list( const ConfigData& config ) {        
        if( config.listSpec & List::Tests )
            listTests( config );
        else if( config.listSpec & List::Reports )
            listReporters( config );
        else
            throw std::logic_error( "Unknown list type" );
    }
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_LIST_HPP_INCLUDED
