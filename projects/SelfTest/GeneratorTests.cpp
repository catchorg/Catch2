/*
 *  GeneratorTests.cpp
 *  Catch - Test
 *
 *  Created by Phil on 28/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "catch.hpp"

inline size_t multiply( size_t a, size_t b )
{
    return a*b;
}

TEST_CASE( "./succeeding/generators/1", "Generators over two ranges" )
{
    using namespace Catch::Generators;
    
    size_t i = GENERATE( between( 1, 5 ).then( values( 15, 20, 21 ).then( 36 ) ) );
    size_t j = GENERATE( between( 100, 107 ) );
    
    REQUIRE( multiply( i, 2 ) == i*2 );
    REQUIRE( multiply( j, 2 ) == j*2 );
}
