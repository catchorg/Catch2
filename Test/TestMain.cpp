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

#include "../internal/catch_self_test.hpp"
#include "../catch_default_main.hpp"

TEST_CASE( "selftest/main", "Runs all Catch self tests and checks their results" )
{
    using namespace Catch;
    
    EmbeddedRunner runner;

    runner.runMatching( "./succeeding/*" );
    CHECK( runner.getReporter().getSucceeded() == 53 );
    CHECK( runner.getReporter().getFailed() == 0 );

    runner.runMatching( "./failing/*" );
        
    CHECK( runner.getReporter().getSucceeded() == 0 );
    CHECK( runner.getReporter().getFailed() == 53 );
}
