// 01-TestCase.cpp

// Let Catch provide main():
#define CATCH_CONFIG_MAIN

#include "catch.hpp"

int Factorial( int number ) {
   return number <= 1 ? number : Factorial( number - 1 ) * number;  // fail
// return number <= 1 ? 1      : Factorial( number - 1 ) * number;  // pass
}

TEST_CASE( "Factorial of 0 is computed (fail)", "[single-file]" ) {
    REQUIRE( Factorial(0) == 1 );
}

TEST_CASE( "Factorials of 1 and higher are computed (pass)", "[single-file]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}

// Compile & run:
// - g++ -std=c++11 -Wall -I$(CATCH_SINGLE_INCLUDE) -o 01-TestCase 01-TestCase.cpp && 01-TestCase --success
// - cl -EHsc -I%CATCH_SINGLE_INCLUDE% 01-TestCase.cpp && 01-TestCase --success

// Expected compact output (all assertions):
//
// prompt> 01-TestCase --reporter compact --success
// 01-TestCase.cpp:18: failed: Factorial(0) == 1 for: 0 == 1
// 01-TestCase.cpp:22: passed: Factorial(1) == 1 for: 1 == 1
// 01-TestCase.cpp:23: passed: Factorial(2) == 2 for: 2 == 2
// 01-TestCase.cpp:24: passed: Factorial(3) == 6 for: 6 == 6
// 01-TestCase.cpp:25: passed: Factorial(10) == 3628800 for: 3628800 (0x375f00) == 3628800 (0x375f00)
// Failed 1 test case, failed 1 assertion.
