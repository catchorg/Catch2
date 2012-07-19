  #define CATCH_CONFIG_MAIN  // This tell CATCH to provide a main() - only do this in one cpp file
  #include "catch.hpp"

  unsigned int Factorial( unsigned int number ) {
      return number <= 1 ? number : Factorial(number-1)*number;
  }

  TEST_CASE( "example/factorial", "The Factorial function returns the factorial of the number passed in" ) {
      REQUIRE( Factorial(0) == (unsigned) 1 );
      REQUIRE( Factorial(1) == (unsigned) 1 );         
      REQUIRE( Factorial(2) == (unsigned) 2 );
      REQUIRE( Factorial(3) == (unsigned) 6 );
      REQUIRE( Factorial(10) == (unsigned) 3628800 );

  } 
