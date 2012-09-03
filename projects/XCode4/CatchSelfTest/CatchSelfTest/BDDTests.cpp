/*
 *  Created by Phil on 29/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifdef __clang__
#pragma clang diagnostic ignored "-Wpadded"
#endif

#include "catch.hpp"

#define STORY( a, b )
#define SCENARIO( storyName, desc ) TEST_CASE( desc, "" )
#define GIVEN( desc ) SECTION( desc, "" )
#define WHEN( desc ) SECTION( desc, "" )
#define THEN( desc ) SECTION( desc, "" )

inline bool itDoesThis(){ return true; }

STORY( storyName, "once upon a time" )

SCENARIO( storyName, "scenario name" )
{
    GIVEN( "This stuff exists" )
    {
        // make stuff exist

        WHEN( "I do this" )
        {
            // do this

            THEN( "it should this this")
            {
                REQUIRE( itDoesThis() );
            }
        }

    }
}
