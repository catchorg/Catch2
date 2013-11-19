/*
 *  Created by Phil on 28/04/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "Some simple comparisons between doubles",
    "[Approx]"
)
{
    double d = 1.23;
    
    REQUIRE( d == Approx( 1.23 ) );
    REQUIRE( d != Approx( 1.22 ) );
    REQUIRE( d != Approx( 1.24 ) );

    REQUIRE( Approx( d ) == 1.23 );
    REQUIRE( Approx( d ) != 1.22 );
    REQUIRE( Approx( d ) != 1.24 );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "Approximate comparisons with different epsilons",
    "[Approx]"
 )
{
    double d = 1.23;
    
    REQUIRE( d != Approx( 1.231 ) );
    REQUIRE( d == Approx( 1.231 ).epsilon( 0.1 ) );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "Approximate comparisons with floats",
    "[Approx]"
)
{
    REQUIRE( 1.23f == Approx( 1.23f ) );
    REQUIRE( 0.0f == Approx( 0.0f ) );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "Approximate comparisons with ints",
    "[Approx]"
)
{
    REQUIRE( 1 == Approx( 1 ) );
    REQUIRE( 0 == Approx( 0 ) );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "Approximate comparisons with mixed numeric types",
    "[Approx]"
)
{
    const double dZero = 0;
    const double dSmall = 0.00001;
    const double dMedium = 1.234;
    
    REQUIRE( 1.0f == Approx( 1 ) );
    REQUIRE( 0 == Approx( dZero) );
    REQUIRE( 0 == Approx( dSmall ).epsilon( 0.001 ) );
    REQUIRE( 1.234f == Approx( dMedium ) );
    REQUIRE( dMedium == Approx( 1.234f ) );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "Use a custom approx",
    "[Approx][custom]"
)
{
    double d = 1.23;
    
    Approx approx = Approx::custom().epsilon( 0.005 );
    
    REQUIRE( d == approx( 1.23 ) );
    REQUIRE( d == approx( 1.22 ) );
    REQUIRE( d == approx( 1.24 ) );
    REQUIRE( d != approx( 1.25 ) );
    
    REQUIRE( approx( d ) == 1.23 );
    REQUIRE( approx( d ) == 1.22 );
    REQUIRE( approx( d ) == 1.24 );
    REQUIRE( approx( d ) != 1.25 );
}

inline double divide( double a, double b ) {
    return a/b;
}

TEST_CASE( "Approximate PI", "[Approx][PI]" )
{
    REQUIRE( divide( 22, 7 ) == Approx( 3.141 ).epsilon( 0.001 ) );
    REQUIRE( divide( 22, 7 ) != Approx( 3.141 ).epsilon( 0.0001 ) );
}
