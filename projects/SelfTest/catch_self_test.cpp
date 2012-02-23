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

namespace Catch
{
    ///////////////////////////////////////////////////////////////////////////
    std::size_t EmbeddedRunner::runMatching
    (
        const std::string& rawTestSpec
    )
    {
        std::ostringstream oss;
        Config config;
        config.setStreamBuf( oss.rdbuf() );
        config.setReporter( "basic" );
        
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
}
