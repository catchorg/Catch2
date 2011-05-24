/*
 *  ApproxTests.cpp
 *  Catch - Test
 *
 *  Created by Phil on 28/04/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "catch.hpp"

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "./succeeding/Approx/simple", 
    "Some simple comparisons between doubles"
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
    "./succeeding/Approx/epsilon", 
    "Approximate comparisons with different epsilons"
 )
{
    double d = 1.23;
    
    REQUIRE( d != Approx( 1.231 ) );
    REQUIRE( d == Approx( 1.231 ).epsilon( 0.1 ) );
//    REQUIRE( d != Approx( 1.232 ).epsilon( 0.1 ) );
}
