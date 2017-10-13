// 02-TestCase-2.cpp

// main() provided by Catch in file 02-TestCase-1.cpp.

#include "catch.hpp"

int Factorial( int number ) {
   return number <= 1 ? number : Factorial( number - 1 ) * number;  // fail
// return number <= 1 ? 1      : Factorial( number - 1 ) * number;  // pass
}

TEST_CASE( "2: Factorial of 0 is computed (fail)", "[multi-file:2]" ) {
    REQUIRE( Factorial(0) == 1 );
}

TEST_CASE( "2: Factorials of 1 and higher are computed (pass)", "[multi-file:2]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}

// Compile: see 02-TestCase-1.cpp

// Expected compact output (all assertions):
//
// prompt> 02-TestCase --reporter compact --success
// 02-TestCase-2.cpp:15: failed: Factorial(0) == 1 for: 0 == 1
// 02-TestCase-2.cpp:19: passed: Factorial(1) == 1 for: 1 == 1
// 02-TestCase-2.cpp:20: passed: Factorial(2) == 2 for: 2 == 2
// 02-TestCase-2.cpp:21: passed: Factorial(3) == 6 for: 6 == 6
// 02-TestCase-2.cpp:22: passed: Factorial(10) == 3628800 for: 3628800 (0x375f00) == 3628800 (0x375f00)
// Failed 1 test case, failed 1 assertion.
