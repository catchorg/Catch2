/*
 *  Created by Phil on 29/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

// !TBD: story scenarios map to class based tests
#define SCENARIO( name, tags ) TEST_CASE( "Scenario: " name, tags )
#define GIVEN( desc )    SECTION( "Given: " desc, "" )
#define WHEN( desc )     SECTION( " When: " desc, "" )
#define AND_WHEN( desc ) SECTION( "  And: " desc, "" )
#define THEN( desc )     SECTION( " Then: " desc, "" )
#define AND_THEN( desc ) SECTION( "  And: " desc, "" )

inline bool itDoesThis(){ return true; }
inline bool itDoesThat(){ return true; }

SCENARIO( "Do that thing with the thing", "[Tags]" ) {
    GIVEN( "This stuff exists" ) {
        // make stuff exist
        WHEN( "I do this" ) {
            // do this
            THEN( "it should do this")
            {
                REQUIRE( itDoesThis() );
                AND_THEN( "do that")
                    REQUIRE( itDoesThat() );
            }
        }
    }
}
