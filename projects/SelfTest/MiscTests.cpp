/*
 *  Created by Phil on 29/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

#ifdef __clang__
#   pragma clang diagnostic ignored "-Wc++98-compat"
#   pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#endif

#include "../include/internal/catch_xmlwriter.hpp"

#include <iostream>
#include <cerrno>

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
            SUCCEED(""); // avoid failing due to no tests
        }

        SECTION( "e (leaf)", "" )
        {
            SUCCEED(""); // avoid failing due to no tests
        }
    }

    SECTION( "f (leaf)", "" )
    {
        SUCCEED(""); // avoid failing due to no tests
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
    return makeNull ? CATCH_NULL : "valid string";
}

TEST_CASE( "null strings", "" )
{
    REQUIRE( makeString( false ) != static_cast<char*>(CATCH_NULL));
    REQUIRE( makeString( true ) == static_cast<char*>(CATCH_NULL));
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
        SUCCEED(""); // We need this here to stop it failing due to no tests
    }
    SECTION( "encoded chars", "these should all be encoded: &&&\"\"\"<<<&\"<<&\"" )
    {
        SUCCEED(""); // We need this here to stop it failing due to no tests
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


TEST_CASE( "toString on const wchar_t const pointer returns the string contents", "[toString]" ) {
        const wchar_t * const s = L"wide load";
        std::string result = Catch::toString( s );
        CHECK( result == "\"wide load\"" );
}

TEST_CASE( "toString on const wchar_t pointer returns the string contents", "[toString]" ) {
        const wchar_t * s = L"wide load";
        std::string result = Catch::toString( s );
        CHECK( result == "\"wide load\"" );
}

TEST_CASE( "toString on wchar_t const pointer returns the string contents", "[toString]" ) {
        wchar_t * const s = const_cast<wchar_t* const>( L"wide load" );
        std::string result = Catch::toString( s );
        CHECK( result == "\"wide load\"" );
}

TEST_CASE( "toString on wchar_t returns the string contents", "[toString]" ) {
        wchar_t * s = const_cast<wchar_t*>( L"wide load" );
        std::string result = Catch::toString( s );
        CHECK( result == "\"wide load\"" );
}

inline std::string encode( std::string const& str, Catch::XmlEncode::ForWhat forWhat = Catch::XmlEncode::ForTextNodes ) {
    std::ostringstream oss;
    oss << Catch::XmlEncode( str, forWhat );
    return oss.str();
}

TEST_CASE( "XmlEncode" ) {
    SECTION( "normal string" ) {
        REQUIRE( encode( "normal string" ) == "normal string" );
    }
    SECTION( "empty string" ) {
        REQUIRE( encode( "" ) == "" );
    }
    SECTION( "string with ampersand" ) {
        REQUIRE( encode( "smith & jones" ) == "smith &amp; jones" );
    }
    SECTION( "string with less-than" ) {
        REQUIRE( encode( "smith < jones" ) == "smith &lt; jones" );
    }
    SECTION( "string with greater-than" ) {
        REQUIRE( encode( "smith > jones" ) == "smith > jones" );
        REQUIRE( encode( "smith ]]> jones" ) == "smith ]]&gt; jones" );
    }
    SECTION( "string with quotes" ) {
        std::string stringWithQuotes = "don't \"quote\" me on that";
        REQUIRE( encode( stringWithQuotes ) == stringWithQuotes );
        REQUIRE( encode( stringWithQuotes, Catch::XmlEncode::ForAttributes ) == "don't &quot;quote&quot; me on that" );
    }
    SECTION( "string with control char (1)" ) {
        REQUIRE( encode( "[\x01]" ) == "[\\x01]" );
    }
    SECTION( "string with control char (x7F)" ) {
        REQUIRE( encode( "[\x7F]" ) == "[\\x7F]" );
    }
}

#ifdef CATCH_CONFIG_CPP11_LONG_LONG
TEST_CASE( "long long", "[c++11][.]" ) {
    long long l = std::numeric_limits<long long>::max();

    REQUIRE( l == std::numeric_limits<long long>::max() );
}
#endif

//TEST_CASE( "Divide by Zero signal handler", "[.][sig]" ) {
//    int i = 0;
//    int x = 10/i; // This should cause the signal to fire
//    CHECK( x == 0 );
//}

TEST_CASE( "This test 'should' fail but doesn't", "[.][failing][!shouldfail]" )
{
    SUCCEED( "oops!" );
}

TEST_CASE( "# A test name that starts with a #" ) {
    SUCCEED( "yay" );
}


static int f() {
    return 1;
}

TEST_CASE( "#835 -- errno should not be touched by Catch", "[!shouldfail]" ) {
    errno = 1;
    CHECK(f() == 0);
    REQUIRE(errno == 1); // Check that f() doesn't touch errno.
}

TEST_CASE( "#961 -- Dynamically created sections should all be reported", "[.]" ) {
    for (char i = '0'; i < '5'; ++i) {
        SECTION(std::string("Looped section ") + i) {
            SUCCEED( "Everything is OK" );
        }
    }
}
