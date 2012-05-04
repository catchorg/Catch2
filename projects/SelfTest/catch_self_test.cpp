/*
 *  catch_self_test.cpp
 *  Catch
 *
 *  Created by Phil on 14/02/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define CATCH_CONFIG_MAIN
#include "catch_self_test.hpp"

namespace Catch{
    
    std::size_t EmbeddedRunner::runMatching( 
        const std::string& rawTestSpec, 
        const std::string& reporter )
    {
        std::ostringstream oss;
        Config config;
        config.setStreamBuf( oss.rdbuf() );
        
        //if( reporter == "mock" ) // !TBD
            config.setReporter( m_reporter.get() );
        
        std::size_t result;
        
        // Scoped because Runner doesn't report EndTesting until its destructor
        {
            Runner runner( config );
            result = runner.runMatching( rawTestSpec );
            m_totals = runner.getTotals();
        }
        m_output = oss.str();
        return result;
    }

    const std::string MockReporter::recordGroups = "[g]";
    const std::string MockReporter::recordTestCases = "[tc]";
    const std::string MockReporter::recordSections =" [s]";
    
    INTERNAL_CATCH_REGISTER_REPORTER( "mock", MockReporter )
    
}
