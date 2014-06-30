/*
 *  Created by Phil on 29/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

#include <iostream>

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#endif

TEST_CASE( "random SECTION tests", "[.][sections][failing]" )
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

TEST_CASE( "nested SECTION tests", "[.][sections][failing]" )
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

TEST_CASE( "more nested SECTION tests", "[sections][failing][.]" )
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

TEST_CASE( "even more nested SECTION tests", "[sections]" )
{
    SECTION( "c", "" )
    {
        SECTION( "d (leaf)", "" )
        {
        }
        
        SECTION( "e (leaf)", "" )
        {
        }
    }

    SECTION( "f (leaf)", "" )
    {
    }
}

TEST_CASE( "looped SECTION tests", "[.][failing][sections]" )
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

TEST_CASE( "looped tests", "[.][failing]" )
{
    static const int fib[]  = { 1, 1, 2, 3, 5, 8, 13, 21 };
    
    for( size_t i=0; i < sizeof(fib)/sizeof(int); ++i )
    {
        INFO( "Testing if fib[" << i << "] (" << fib[i] << ") is even" );
        CHECK( ( fib[i] % 2 ) == 0 );                
    }
}

TEST_CASE( "Sends stuff to stdout and stderr", "[.]" )
{
    std::cout << "A string sent directly to stdout" << std::endl;
    
    std::cerr << "A string sent directly to stderr" << std::endl;
}

inline const char* makeString( bool makeNull )
{
    return makeNull ? NULL : "valid string";
}

TEST_CASE( "null strings", "" )
{
    REQUIRE( makeString( false ) != static_cast<char*>(NULL));
    REQUIRE( makeString( true ) == static_cast<char*>(NULL));
}


inline bool testCheckedIf( bool flag )
{
    CHECKED_IF( flag )
        return true;
    else
        return false;
}

TEST_CASE( "checkedIf", "" )
{
    REQUIRE( testCheckedIf( true ) );
}

TEST_CASE( "checkedIf, failing", "[failing][.]" )
{
    REQUIRE( testCheckedIf( false ) );
}

inline bool testCheckedElse( bool flag )
{
    CHECKED_ELSE( flag )
        return false;
    
    return true;
}

TEST_CASE( "checkedElse", "" )
{
    REQUIRE( testCheckedElse( true ) );
}

TEST_CASE( "checkedElse, failing", "[failing][.]" )
{
    REQUIRE( testCheckedElse( false ) );
}

TEST_CASE( "xmlentitycheck", "" )
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

TEST_CASE( "send a single char to INFO", "[failing][.]" )
{
    INFO(3);
    REQUIRE(false);    
}

TEST_CASE( "atomic if", "[failing][0]")
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

TEST_CASE("String matchers", "[matchers]" )
{
    REQUIRE_THAT( testStringForMatching(), Contains( "string" ) );    
    CHECK_THAT( testStringForMatching(), Contains( "abc" ) );

    CHECK_THAT( testStringForMatching(), StartsWith( "this" ) );
    CHECK_THAT( testStringForMatching(), EndsWith( "substring" ) );
}

TEST_CASE("Contains string matcher", "[.][failing][matchers]")
{
    CHECK_THAT( testStringForMatching(), Contains( "not there" ) );
}

TEST_CASE("StartsWith string matcher", "[.][failing][matchers]")
{
    CHECK_THAT( testStringForMatching(), StartsWith( "string" ) );
}

TEST_CASE("EndsWith string matcher", "[.][failing][matchers]")
{
    CHECK_THAT( testStringForMatching(), EndsWith( "this" ) );
}

TEST_CASE("Equals string matcher", "[.][failing][matchers]")
{
    CHECK_THAT( testStringForMatching(), Equals( "something else" ) );
}
TEST_CASE("Equals string matcher, with NULL", "[matchers]")
{
    REQUIRE_THAT("", Equals(NULL));
}
TEST_CASE("AllOf matcher", "[matchers]")
{
    CHECK_THAT( testStringForMatching(), AllOf( Catch::Contains( "string" ), Catch::Contains( "abc" ) ) );
}
TEST_CASE("AnyOf matcher", "[matchers]")
{
    CHECK_THAT( testStringForMatching(), AnyOf( Catch::Contains( "string" ), Catch::Contains( "not there" ) ) );
    CHECK_THAT( testStringForMatching(), AnyOf( Catch::Contains( "not there" ), Catch::Contains( "string" ) ) );
}

TEST_CASE("Equals", "[matchers]")
{
    CHECK_THAT( testStringForMatching(), Equals( "this string contains 'abc' as a substring" ) );
}

inline unsigned int Factorial( unsigned int number )
{
//  return number <= 1 ? number : Factorial(number-1)*number;
  return number > 1 ? Factorial(number-1)*number : 1;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
  REQUIRE( Factorial(0) == 1 );
  REQUIRE( Factorial(1) == 1 );
  REQUIRE( Factorial(2) == 2 );
  REQUIRE( Factorial(3) == 6 );
  REQUIRE( Factorial(10) == 3628800 );
}

TEST_CASE( "An empty test with no assertions", "[empty]" )
{
}

TEST_CASE( "Nice descriptive name", "[tag1][tag2][tag3][.]" )
{
    WARN( "This one ran" );
}
TEST_CASE( "first tag", "[tag1]" )
{
}
TEST_CASE( "second tag", "[tag2]" )
{
}
//
//TEST_CASE( "spawn a new process", "[.]" )
//{
//    // !TBD Work in progress
//    char line[200];
//    FILE* output = popen("./CatchSelfTest ./failing/matchers/StartsWith", "r");
//    while ( fgets(line, 199, output) )
//        std::cout << line;
//}

TEST_CASE( "vectors can be sized and resized", "[vector]" ) {

    std::vector<int> v( 5 );
    
    REQUIRE( v.size() == 5 );
    REQUIRE( v.capacity() >= 5 );
    
    SECTION( "resizing bigger changes size and capacity", "" ) {
        v.resize( 10 );
        
        REQUIRE( v.size() == 10 );
        REQUIRE( v.capacity() >= 10 );
    }
    SECTION( "resizing smaller changes size but not capacity", "" ) {
        v.resize( 0 );
        
        REQUIRE( v.size() == 0 );
        REQUIRE( v.capacity() >= 5 );
        
        SECTION( "We can use the 'swap trick' to reset the capacity", "" ) {
            std::vector<int> empty;
            empty.swap( v );
            
            REQUIRE( v.capacity() == 0 );
        }
    }
    SECTION( "reserving bigger changes capacity but not size", "" ) {
        v.reserve( 10 );
        
        REQUIRE( v.size() == 5 );
        REQUIRE( v.capacity() >= 10 );
    }
    SECTION( "reserving smaller does not change size or capacity", "" ) {
        v.reserve( 0 );
        
        REQUIRE( v.size() == 5 );
        REQUIRE( v.capacity() >= 5 );
    }
}

// https://github.com/philsquared/Catch/issues/166
TEST_CASE("A couple of nested sections followed by a failure", "[failing][.]")
{
    SECTION("Outer", "")
        SECTION("Inner", "")
            SUCCEED("that's not flying - that's failing in style");

    FAIL("to infinity and beyond");
}

TEST_CASE("not allowed", "[!throws]")
{
    // This test case should not be included if you run with -e on the command line
    SUCCEED( "" );
}

//TEST_CASE( "Is big endian" ) {
//    CHECK( Catch::Detail::Endianness::which() == Catch::Detail::Endianness::Little );
//}

TEST_CASE( "Tabs and newlines show in output", "[.][whitespace][failing]" ) {

    // Based on issue #242
    std::string s1 = "if ($b == 10) {\n\t\t$a\t= 20;\n}";
    std::string s2 = "if ($b == 10) {\n\t$a = 20;\n}\n";
    CHECK( s1 == s2 );
}

