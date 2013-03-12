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
#include <limits>

namespace Catch {
    inline bool matchesFilters( const std::vector<TestCaseFilters>& filters, const TestCaseInfo& testCase ) {
        std::vector<TestCaseFilters>::const_iterator it = filters.begin();
        std::vector<TestCaseFilters>::const_iterator itEnd = filters.end();
        for(; it != itEnd; ++it )
            if( !it->shouldInclude( testCase ) )
                return false;
        return true;
    }
    inline void List( const ConfigData& config ) {
        
        if( config.listSpec & List::Reports ) {
            std::cout << "Available reports:\n";
            IReporterRegistry::FactoryMap::const_iterator it = getRegistryHub().getReporterRegistry().getFactories().begin();
            IReporterRegistry::FactoryMap::const_iterator itEnd = getRegistryHub().getReporterRegistry().getFactories().end();
            for(; it != itEnd; ++it ) {
                // !TBD: consider listAs()
                std::cout << "\t" << it->first << "\n\t\t'" << it->second->getDescription() << "'\n";
            }
            std::cout << std::endl;
        }
        
        if( config.listSpec & List::Tests ) {
            if( config.filters.empty() )
                std::cout << "All available test cases:\n";
            else
                std::cout << "Matching test cases:\n";
            std::vector<TestCaseInfo>::const_iterator it = getRegistryHub().getTestCaseRegistry().getAllTests().begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = getRegistryHub().getTestCaseRegistry().getAllTests().end();
            std::size_t matchedTests = 0;
            for(; it != itEnd; ++it ) {
                if( matchesFilters( config.filters, *it ) ) {
                    matchedTests++;
                    // !TBD: consider listAs()
                    std::cout << "\t" << it->getName() << "\n";
                    if( ( config.listSpec & List::TestNames ) != List::TestNames )
                        std::cout << "\t\t '" << it->getDescription() << "'\n";
                }
            }
            if( config.filters.empty() )
                std::cout << pluralise( matchedTests, "test case" ) << std::endl;
            else
                std::cout << pluralise( matchedTests, "matching test case" ) << std::endl;
        }
        
        if( ( config.listSpec & List::All ) == 0 ) {
            std::ostringstream oss;
            oss << "Unknown list type";
            throw std::domain_error( oss.str() );
        }
    }
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_LIST_HPP_INCLUDED
