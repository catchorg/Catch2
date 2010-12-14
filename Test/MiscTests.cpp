/*
 *  MiscTests.cpp
 *  Catch - Test
 *
 *  Created by Phil on 29/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "../catch.hpp"

TEST_CASE( "succeeding/Misc/Sections", "random SECTION tests" )
{
    int a = 1;
    int b = 2;
    
    SECTION( "s1", "doesn't equal" )
    {
        REQUIRE( a != b );
        REQUIRE( b != a );
    }

    SECTION( "s2", "not equal" )
    {
        REQUIRE_FALSE( a == b);
    }
}

TEST_CASE( "succeeding/Misc/stdout,stderr", "Sends stuff to stdout and stderr" )
{
    std::cout << "Some information";
    
    std::cerr << "An error";
}