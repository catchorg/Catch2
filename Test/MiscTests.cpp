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
#include <iostream>

TEST_CASE( "./succeeding/Misc/Sections", "random SECTION tests" )
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

TEST_CASE( "./succeeding/Misc/Sections/nested", "nested SECTION tests" )
{
    int a = 1;
    int b = 2;
    
    SECTION( "s1", "doesn't equal" )
    {
        REQUIRE( a != b );
        REQUIRE( b != a );

        SECTION( "s2", "not equal" )
        {
            REQUIRE_FALSE( a == b);
        }
    }
}

TEST_CASE( "./mixed/Misc/Sections/loops", "looped SECTION tests" )
{
    int a = 1;
    
    for( int b = 0; b < 10; ++b )
    {
        std::ostringstream oss;
        oss << "b is currently: " << b;
        SECTION( "s1", oss.str() )
        {
            CHECK( b > a );            
        }
    }
}

TEST_CASE( "./mixed/Misc/loops", "looped tests" )
{
    static const int fib[]  = { 1, 1, 2, 3, 5, 8, 13, 21 };
    
    for( size_t i=0; i < sizeof(fib)/sizeof(int); ++i )
    {
        INFO( "Testing if fib[" << i << "] (" << fib[i] << ") is even" );
        CHECK( ( fib[i] % 2 ) == 0 );                
    }
}

TEST_CASE( "./succeeding/Misc/stdout,stderr", "Sends stuff to stdout and stderr" )
{
    std::cout << "Some information";
    
    std::cerr << "An error";
}

size_t multiply( int a, int b )
{
    return a*b;
}


TEST_CASE( "./succeeding/generators/1", "Generators over two ranges" )
{
    using namespace Catch::Generators;
    
    size_t i = GENERATE( between( 1, 5 ).then( values( 15, 20, 21 ).then( 36 ) ) );
    size_t j = GENERATE( between( 100, 107 ) );
    
    REQUIRE( multiply( i, 2 ) == i*2 );
    REQUIRE( multiply( j, 2 ) == j*2 );
}
