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


#include <iostream>
#include <cerrno>
#include <limits>
#include <sstream>

namespace { namespace MiscTests {

#ifndef MISC_TEST_HELPERS_INCLUDED // Don't compile this more than once per TU
#define MISC_TEST_HELPERS_INCLUDED

inline const char* makeString( bool makeNull ) {
    return makeNull ? nullptr : "valid string";
}
inline bool testCheckedIf( bool flag )  {
    CHECKED_IF( flag )
        return true;
    else
        return false;
}
inline bool testCheckedElse( bool flag ) {
    CHECKED_ELSE( flag )
        return false;

    return true;
}

inline unsigned int Factorial( unsigned int number )  {
    return number > 1 ? Factorial(number-1)*number : 1;
}

static int f() {
    return 1;
}

inline void manuallyRegisteredTestFunction() {
    SUCCEED( "was called" );
}

struct AutoTestReg {
    AutoTestReg() {
        REGISTER_TEST_CASE( manuallyRegisteredTestFunction, "ManuallyRegistered" );
    }
};
CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS
static AutoTestReg autoTestReg;
CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS

#endif

TEST_CASE( "random SECTION tests", "[.][sections][failing]" ) {
    int a = 1;
    int b = 2;

    SECTION( "doesn't equal" ) {
        REQUIRE( a != b );
        REQUIRE( b != a );
    }

    SECTION( "not equal" ) {
        REQUIRE( a != b);
    }
}

TEST_CASE( "nested SECTION tests", "[.][sections][failing]" ) {
    int a = 1;
    int b = 2;

    SECTION( "doesn't equal" ) {
        REQUIRE( a != b );
        REQUIRE( b != a );

        SECTION( "not equal" ) {
            REQUIRE( a != b);
        }
    }
}

TEST_CASE( "more nested SECTION tests", "[sections][failing][.]" ) {
    int a = 1;
    int b = 2;

    SECTION( "doesn't equal" ) {
        SECTION( "equal" ) {
            REQUIRE( a == b );
        }

        SECTION( "not equal" ) {
            REQUIRE( a != b );
        }
        SECTION( "less than" ) {
            REQUIRE( a < b );
        }
    }
}

TEST_CASE( "even more nested SECTION tests", "[sections]" ) {
    SECTION( "c" ) {
        SECTION( "d (leaf)" ) {
            SUCCEED(); // avoid failing due to no tests
        }

        SECTION( "e (leaf)" ) {
            SUCCEED(); // avoid failing due to no tests
        }
    }

    SECTION( "f (leaf)" ) {
        SUCCEED(); // avoid failing due to no tests
    }
}

TEST_CASE( "looped SECTION tests", "[.][failing][sections]" ) {
    int a = 1;

    for( int b = 0; b < 10; ++b ) {
        DYNAMIC_SECTION( "b is currently: " << b ) {
            CHECK( b > a );
        }
    }
}

TEST_CASE( "looped tests", "[.][failing]" ) {
    static const int fib[]  = { 1, 1, 2, 3, 5, 8, 13, 21 };

    for( std::size_t i=0; i < sizeof(fib)/sizeof(int); ++i ) {
        INFO( "Testing if fib[" << i << "] (" << fib[i] << ") is even" );
        CHECK( ( fib[i] % 2 ) == 0 );
    }
}

TEST_CASE( "Sends stuff to stdout and stderr", "[.]" ) {
    std::cout << "A string sent directly to stdout" << std::endl;
    std::cerr << "A string sent directly to stderr" << std::endl;
    std::clog << "A string sent to stderr via clog" << std::endl;
}

TEST_CASE( "null strings" ) {
    REQUIRE( makeString( false ) != static_cast<char*>(nullptr));
    REQUIRE( makeString( true ) == static_cast<char*>(nullptr));
}

TEST_CASE( "checkedIf" ) {
    REQUIRE( testCheckedIf( true ) );
}

TEST_CASE( "checkedIf, failing", "[failing][.]" ) {
    REQUIRE( testCheckedIf( false ) );
}

TEST_CASE( "checkedElse" ) {
    REQUIRE( testCheckedElse( true ) );
}

TEST_CASE( "checkedElse, failing", "[failing][.]" ) {
    REQUIRE( testCheckedElse( false ) );
}

TEST_CASE( "xmlentitycheck" ) {
    SECTION( "embedded xml: <test>it should be possible to embed xml characters, such as <, \" or &, or even whole <xml>documents</xml> within an attribute</test>" ) {
        SUCCEED(); // We need this here to stop it failing due to no tests
    }
    SECTION( "encoded chars: these should all be encoded: &&&\"\"\"<<<&\"<<&\"" ) {
        SUCCEED(); // We need this here to stop it failing due to no tests
    }
}

TEST_CASE( "send a single char to INFO", "[failing][.]" ) {
    INFO(3);
    REQUIRE(false);
}

TEST_CASE( "atomic if", "[failing][0]") {
    std::size_t x = 0;

    if( x )
        REQUIRE(x > 0);
    else
        REQUIRE(x == 0);
}


TEST_CASE( "Factorials are computed", "[factorial]" ) {
  REQUIRE( Factorial(0) == 1 );
  REQUIRE( Factorial(1) == 1 );
  REQUIRE( Factorial(2) == 2 );
  REQUIRE( Factorial(3) == 6 );
  REQUIRE( Factorial(10) == 3628800 );
}

TEST_CASE( "An empty test with no assertions", "[empty]" ) {}

TEST_CASE( "Nice descriptive name", "[tag1][tag2][tag3][.]" ) {
    WARN( "This one ran" );
}
TEST_CASE( "first tag", "[tag1]" ) {}
TEST_CASE( "second tag", "[tag2]" ) {}

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

    SECTION( "resizing bigger changes size and capacity" ) {
        v.resize( 10 );

        REQUIRE( v.size() == 10 );
        REQUIRE( v.capacity() >= 10 );
    }
    SECTION( "resizing smaller changes size but not capacity" ) {
        v.resize( 0 );

        REQUIRE( v.size() == 0 );
        REQUIRE( v.capacity() >= 5 );

        SECTION( "We can use the 'swap trick' to reset the capacity" ) {
            std::vector<int> empty;
            empty.swap( v );

            REQUIRE( v.capacity() == 0 );
        }
    }
    SECTION( "reserving bigger changes capacity but not size" ) {
        v.reserve( 10 );

        REQUIRE( v.size() == 5 );
        REQUIRE( v.capacity() >= 10 );
    }
    SECTION( "reserving smaller does not change size or capacity" ) {
        v.reserve( 0 );

        REQUIRE( v.size() == 5 );
        REQUIRE( v.capacity() >= 5 );
    }
}

// https://github.com/philsquared/Catch/issues/166
TEST_CASE("A couple of nested sections followed by a failure", "[failing][.]") {
    SECTION("Outer")
        SECTION("Inner")
            SUCCEED("that's not flying - that's failing in style");

    FAIL("to infinity and beyond");
}

TEST_CASE("not allowed", "[!throws]") {
    // This test case should not be included if you run with -e on the command line
    SUCCEED();
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


#ifdef CATCH_CONFIG_WCHAR
TEST_CASE( "toString on const wchar_t const pointer returns the string contents", "[toString]" ) {
        const wchar_t * const s = L"wide load";
        std::string result = ::Catch::Detail::stringify( s );
        CHECK( result == "\"wide load\"" );
}

TEST_CASE( "toString on const wchar_t pointer returns the string contents", "[toString]" ) {
        const wchar_t * s = L"wide load";
        std::string result = ::Catch::Detail::stringify( s );
        CHECK( result == "\"wide load\"" );
}

TEST_CASE( "toString on wchar_t const pointer returns the string contents", "[toString]" ) {
        auto const s = const_cast<wchar_t*>( L"wide load" );
        std::string result = ::Catch::Detail::stringify( s );
        CHECK( result == "\"wide load\"" );
}

TEST_CASE( "toString on wchar_t returns the string contents", "[toString]" ) {
        auto s = const_cast<wchar_t*>( L"wide load" );
        std::string result = ::Catch::Detail::stringify( s );
        CHECK( result == "\"wide load\"" );
}
#endif

TEST_CASE( "long long" ) {
    long long l = std::numeric_limits<long long>::max();

    REQUIRE( l == std::numeric_limits<long long>::max() );
}

//TEST_CASE( "Divide by Zero signal handler", "[.][sig]" ) {
//    int i = 0;
//    int x = 10/i; // This should cause the signal to fire
//    CHECK( x == 0 );
//}

TEST_CASE( "This test 'should' fail but doesn't", "[.][failing][!shouldfail]" ) {
    SUCCEED( "oops!" );
}

TEST_CASE( "# A test name that starts with a #" ) {
    SUCCEED( "yay" );
}

TEST_CASE( "#835 -- errno should not be touched by Catch", "[.][failing][!shouldfail]" ) {
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

TEST_CASE( "#1175 - Hidden Test", "[.]" ) {
  // Just for checking that hidden test is not listed by default
  SUCCEED();
}

}} // namespace MiscTests
