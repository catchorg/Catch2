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

#include "catch_self_test.hpp"

TEST_CASE( "selftest/main", "Runs all Catch self tests and checks their results" )
{
    using namespace Catch;
    
    ///////////////////////////////////////////////////////////////////////////
    SECTION(    "selftest/expected result",
                "Tests do what they claim" )
    {
        SECTION(    "selftest/expected result/failing tests", 
                    "Tests in the 'failing' branch fail" )
        {
            MetaTestRunner::runMatching( "./failing/*",  MetaTestRunner::Expected::ToFail );
        }
        
        SECTION(    "selftest/expected result/succeeding tests", 
                    "Tests in the 'succeeding' branch succeed" )
        {
            MetaTestRunner::runMatching( "./succeeding/*",  MetaTestRunner::Expected::ToSucceed );
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    SECTION(    "selftest/test counts", 
                "Number of test cases that run is fixed" )
    {
        EmbeddedRunner runner;
        
        SECTION(    "selftest/test counts/succeeding tests", 
                    "Number of 'succeeding' tests is fixed" )
        {
            runner.runMatching( "./succeeding/*" );
            CHECK( runner.getTotals().assertions.passed == 272 );
            CHECK( runner.getTotals().assertions.failed == 0 );
        }

        SECTION(    "selftest/test counts/failing tests", 
                    "Number of 'failing' tests is fixed" )
        {
            runner.runMatching( "./failing/*" );        
            CHECK( runner.getTotals().assertions.passed == 0 );
            CHECK( runner.getTotals().assertions.failed == 71 );
        }
    }
}

TEST_CASE( "meta/Misc/Sections", "looped tests" )
{
    Catch::EmbeddedRunner runner;
    
    runner.runMatching( "./mixed/Misc/Sections/nested2" );
    CHECK( runner.getTotals().assertions.passed == 2 );
    CHECK( runner.getTotals().assertions.failed == 1 );
    
}
