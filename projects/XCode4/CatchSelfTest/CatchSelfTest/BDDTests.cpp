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

// !TBD: story scenarios map to class based tests
#define SCENARIO( name, tags ) TEST_CASE( "Scenario: " name, tags )
#define GIVEN( desc ) SECTION( " Given: " desc, "" )
#define WHEN( desc ) SECTION( "  When:  " desc, "" )
#define THEN( desc ) SECTION( "  Then:  " desc, "" )

inline bool itDoesThis(){ return true; }

SCENARIO( "Do that thing with the thing", "[tags]" ) {
    GIVEN( "This stuff exists" ) {
        // make stuff exist
        WHEN( "I do this" ) {
            // do this
            THEN( "it should do this")
                REQUIRE( itDoesThis() );
        }
        
    }
}
