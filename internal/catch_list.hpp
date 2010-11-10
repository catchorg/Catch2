/*
 *  catch_list.hpp
 *  Catch
 *
 *  Created by Phil on 5/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef TWOBLUECUBES_CATCH_LIST_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_LIST_HPP_INCLUDED

#include "catch_commandline.hpp"

namespace Catch
{
    inline int List( const RunnerConfig& config )
    {
        if( config.listWhat() & RunnerConfig::listReports )
        {
            std::cout << "Available reports:\n";
            ReporterRegistry::FactoryMap::const_iterator it = ReporterRegistry::instance().getFactories().begin();
            ReporterRegistry::FactoryMap::const_iterator itEnd = ReporterRegistry::instance().getFactories().end();
            for(; it != itEnd; ++it )
            {
                // !TBD: consider listAs()
                std::cout << "\t" << it->first << " '" << it->second->getDescription() << "'\n";
            }
            std::cout << std::endl;
        }
        if( config.listWhat() & RunnerConfig::listTests )
        {
            std::cout << "Available tests:\n";
            std::vector<TestCaseInfo>::const_iterator it = TestRegistry::instance().getAllTests().begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = TestRegistry::instance().getAllTests().end();
            for(; it != itEnd; ++it )
            {
                // !TBD: consider listAs()
                std::cout << "\t" << it->getName() << " '" << it->getDescription() << "'\n";
            }
            std::cout << std::endl;
        }
        if( ( config.listWhat() & RunnerConfig::listAll ) == 0 )
        {
            std::cerr << "Unknown list type" << std::endl;
            return std::numeric_limits<int>::max();
        }
        
        if( config.getReporter() )
        {
            std::cerr << "Reporters ignored when listing" << std::endl;
        }
        if( config.m_testSpecs.size() == 0 )
        {
            std::cerr << "Test specs ignored when listing" << std::endl;
        }
        return 0;

    }
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_LIST_HPP_INCLUDED