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
    inline int List( Config& config ) {
        
        if( config.listWhat() & List::Reports ) {
            std::cout << "Available reports:\n";
            IReporterRegistry::FactoryMap::const_iterator it = getRegistryHub().getReporterRegistry().getFactories().begin();
            IReporterRegistry::FactoryMap::const_iterator itEnd = getRegistryHub().getReporterRegistry().getFactories().end();
            for(; it != itEnd; ++it ) {
                // !TBD: consider listAs()
                std::cout << "\t" << it->first << "\n\t\t'" << it->second->getDescription() << "'\n";
            }
            std::cout << std::endl;
        }
        
        if( config.listWhat() & List::Tests ) {
            std::cout << "Available tests:\n";
            std::vector<TestCaseInfo>::const_iterator it = getRegistryHub().getTestCaseRegistry().getAllTests().begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = getRegistryHub().getTestCaseRegistry().getAllTests().end();
            for(; it != itEnd; ++it ) {
                // !TBD: consider listAs()
                std::cout << "\t" << it->getName() << "\n\t\t '" << it->getDescription() << "'\n";
            }
            std::cout << std::endl;
        }
        
        if( ( config.listWhat() & List::All ) == 0 ) {
            std::cerr << "Unknown list type" << std::endl;
            return (std::numeric_limits<int>::max)();
        }
        
        return 0;
    }
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_LIST_HPP_INCLUDED
