/*
 *  main.cpp
 *  Catch - Test
 *
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "../catch_with_main.hpp"

#include "../internal/catch_self_test.hpp"

TEST_CASE( "selftest/main", "Runs all Catch self tests and checks their results" )
{
    using namespace Catch;
    
    EmbeddedRunner runner;

    runner.runMatching( "./succeeding/*" );
    INFO( runner.getOutput() );
    CHECK( runner.getSuccessCount() == 196 );
    CHECK( runner.getFailureCount() == 0 );

    runner.runMatching( "./failing/*" );        
    CHECK( runner.getSuccessCount() == 0 );
    CHECK( runner.getFailureCount() == 53 );
}
