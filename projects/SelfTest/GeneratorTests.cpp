/*
 *  Created by Phil on 28/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// This define means we have to prefix all the CATCH macros with CATCH_
// We're using it here to test it out
#define CATCH_CONFIG_PREFIX_ALL
#include "catch.hpp"

namespace GeneratorTests
{
    inline int multiply( int a, int b )
    {
        return a*b;
    }

    CATCH_TEST_CASE( "Generators over two ranges", "[generators]" )
    {
        using namespace Catch::Generators;
    
        int i = CATCH_GENERATE( between( 1, 5 ).then( values( 15, 20, 21 ).then( 36 ) ) );
        int j = CATCH_GENERATE( between( 100, 107 ) );
    
        CATCH_REQUIRE( multiply( i, 2 ) == i*2 );
        CATCH_REQUIRE( multiply( j, 2 ) == j*2 );
    }

    struct IntPair { int first, second; };

    CATCH_TEST_CASE( "Generator over a range of pairs", "[generators]" )
    {
        using namespace Catch::Generators;
 
        IntPair p[] = { { 0, 1 }, { 2, 3 } };
    
        IntPair* i = CATCH_GENERATE( between( p, &p[1] ) );
    
        CATCH_REQUIRE( i->first == i->second-1 );
    
    }
}
