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

#include "catch.hpp"
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
        REQUIRE( a != b);
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
            REQUIRE( a != b);
        }
    }
}

TEST_CASE( "./mixed/Misc/Sections/nested2", "nested SECTION tests" )
{
    int a = 1;
    int b = 2;
    
    SECTION( "s1", "doesn't equal" )
    {
        SECTION( "s2", "equal" )
        {
            REQUIRE( a == b );
        }

        SECTION( "s3", "not equal" )
        {
            REQUIRE( a != b );
        }
        SECTION( "s4", "less than" )
        {
            REQUIRE( a < b );
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
    std::cout << "Some information" << std::endl;
    
    std::cerr << "An error" << std::endl;
}

inline const char* makeString( bool makeNull )
{
    return makeNull ? NULL : "valid string";
}

TEST_CASE( "./succeeding/Misc/null strings", "" )
{
    REQUIRE( makeString( false ) != static_cast<char*>(NULL));
    REQUIRE( makeString( true ) == static_cast<char*>(NULL));
}

TEST_CASE( "./failing/info", "sends information to INFO" )
{
    INFO( "hi" );
    int i = 7;
    CAPTURE( i );
    REQUIRE( false );
}

inline bool testCheckedIf( bool flag )
{
    CHECKED_IF( flag )
        return true;
    else
        return false;
}

TEST_CASE( "./succeeding/checkedif", "" )
{
    REQUIRE( testCheckedIf( true ) );
}

TEST_CASE( "./failing/checkedif", "" )
{
    REQUIRE( testCheckedIf( false ) );
}

inline bool testCheckedElse( bool flag )
{
    CHECKED_ELSE( flag )
        return false;
    
    return true;
}

TEST_CASE( "./succeeding/checkedelse", "" )
{
    REQUIRE( testCheckedElse( true ) );
}

TEST_CASE( "./failing/checkedelse", "" )
{
    REQUIRE( testCheckedElse( false ) );
}

TEST_CASE( "./misc/xmlentitycheck", "" )
{
    SECTION( "embedded xml", "<test>it should be possible to embed xml characters, such as <, \" or &, or even whole <xml>documents</xml> within an attribute</test>" )
    {
        // No test
    }
    SECTION( "encoded chars", "these should all be encoded: &&&\"\"\"<<<&\"<<&\"" )
    {
        // No test
    }
}

TEST_CASE( "./manual/onechar", "send a single char to INFO" )
{
    INFO(3);
    REQUIRE(false);    
}

TEST_CASE("./succeeding/atomic if", "")
{
    size_t x = 0;
    
    if( x )
        REQUIRE(x > 0);
    else
        REQUIRE(x == 0);
}

inline const char* testStringForMatching()
{
    return "this string contains 'abc' as a substring";
}

using namespace Catch::Matchers;

TEST_CASE("./succeeding/matchers", "") 
{    
    REQUIRE_THAT( testStringForMatching(), Contains( "string" ) );    
    CHECK_THAT( testStringForMatching(), Contains( "abc" ) );

    CHECK_THAT( testStringForMatching(), StartsWith( "this" ) );
    CHECK_THAT( testStringForMatching(), EndsWith( "substring" ) );
}

TEST_CASE("./failing/matchers/Contains", "") 
{
    CHECK_THAT( testStringForMatching(), Contains( "not there" ) );
}

TEST_CASE("./failing/matchers/StartsWith", "") 
{
    CHECK_THAT( testStringForMatching(), StartsWith( "string" ) );
}

TEST_CASE("./failing/matchers/EndsWith", "") 
{
    CHECK_THAT( testStringForMatching(), EndsWith( "this" ) );
}
