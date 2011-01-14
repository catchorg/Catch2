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
    
    {
        EmbeddedRunner runner;
        runner.runMatching( "succeeding/*" );
        CHECK( runner.getReporter().getSucceeded() == 53 );
        CHECK( runner.getReporter().getFailed() == 0 );
    }
    {
        EmbeddedRunner runner;
        runner.runMatching( "failing/*" );
        
        CHECK( runner.getReporter().getSucceeded() == 0 );
        CHECK( runner.getReporter().getFailed() == 53 );
    }
}

TEST_CASE( "selftest/succeeding", "Runs all Catch self tests that should succeed and checks their results" )
{
    using namespace Catch;
    
    // Run a nested Runner - we scope it here so it restores our runner
    // at the end of scope
    {
        SelfTestConfig config;
        {
            Runner runner( config );
            runner.runMatching( "succeeding/*" );
        }
        
        CHECK( config.getReporter().getSucceeded() == 53 );
        CHECK( config.getReporter().getFailed() == 0 );
    }
}

TEST_CASE( "selftest/failing", "Runs all Catch self tests that should fail and checks their results" )
{
    using namespace Catch;
    
    // Run a nested Runner - we scope it here so it restores our runner
    // at the end of scope
    {
        SelfTestConfig config;
        {
            Runner runner( config );
            runner.runMatching( "failing/*" );
        }
        
        CHECK( config.getReporter().getSucceeded() == 0 );
        CHECK( config.getReporter().getFailed() == 53 );
    }
}
