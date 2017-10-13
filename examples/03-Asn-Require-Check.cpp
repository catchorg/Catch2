// 03-Asn-Require-Check.cpp

// Catch has two natural expression assertion macro's:
// - REQUIRE() stops at first failure.
// - CHECK() continues after failure.

// There are two variants to support decomposing negated expressions:
// - REQUIRE_FALSE() stops at first failure.
// - CHECK_FALSE() continues after failure.

// Let Catch provide a main():
#define CATCH_CONFIG_MAIN

#include "catch.hpp"

std::string one() {
    return "1";
}

TEST_CASE( "Assert that something is true (pass)", "[require]" ) {
    REQUIRE( one() == "1" );
}

TEST_CASE( "Assert that something is true (fail)", "[require]" ) {
    REQUIRE( one() == "x" );
}

TEST_CASE( "Assert that something is true (stop at first failure)", "[require]" ) {
    WARN( "REQUIRE stops at first failure:" );

    REQUIRE( one() == "x" );
    REQUIRE( one() == "1" );
}

TEST_CASE( "Assert that something is true (continue after failure)", "[check]" ) {
    WARN( "CHECK continues after failure:" );

    CHECK(   one() == "x" );
    REQUIRE( one() == "1" );
}

TEST_CASE( "Assert that something is false (stops at first failure)", "[require-false]" ) {
    WARN( "REQUIRE_FALSE stops at first failure:" );

    REQUIRE_FALSE( one() == "1" );
    REQUIRE_FALSE( one() != "1" );
}

TEST_CASE( "Assert that something is false (continue after failure)", "[check-false]" ) {
    WARN( "CHECK_FALSE continues after failure:" );

    CHECK_FALSE(   one() == "1" );
    REQUIRE_FALSE( one() != "1" );
}

// Compile & run:
// - g++ -std=c++11 -Wall -I$(CATCH_SINGLE_INCLUDE) -o 03-Asn-Require-Check 03-Asn-Require-Check.cpp && 03-Asn-Require-Check --success
// - cl -EHsc -I%CATCH_SINGLE_INCLUDE% 03-Asn-Require-Check.cpp && 03-Asn-Require-Check --success

// Expected compact output (all assertions):
//
// prompt> 03-Asn-Require-Check.exe --reporter compact --success
// 03-Asn-Require-Check.cpp:22: passed: one() == "1" for: "1" == "1"
// 03-Asn-Require-Check.cpp:26: failed: one() == "x" for: "1" == "x"
// 03-Asn-Require-Check.cpp:30: warning: 'REQUIRE stops at first failure:'
// 03-Asn-Require-Check.cpp:32: failed: one() == "x" for: "1" == "x"
// 03-Asn-Require-Check.cpp:37: warning: 'CHECK continues after failure:'
// 03-Asn-Require-Check.cpp:39: failed: one() == "x" for: "1" == "x"
// 03-Asn-Require-Check.cpp:40: passed: one() == "1" for: "1" == "1"
// 03-Asn-Require-Check.cpp:44: warning: 'REQUIRE_FALSE stops at first failure:'
// 03-Asn-Require-Check.cpp:46: failed: !(one() == "1") for: !("1" == "1")
// 03-Asn-Require-Check.cpp:51: warning: 'CHECK_FALSE continues after failure:'
// 03-Asn-Require-Check.cpp:53: failed: !(one() == "1") for: !("1" == "1")
// 03-Asn-Require-Check.cpp:54: passed: !(one() != "1") for: !("1" != "1")
// Failed 5 test cases, failed 5 assertions.
