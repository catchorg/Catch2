/*
 *  OCTest.mm
 *  OCTest
 *
 *  Created by Phil on 13/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#import "catch.hpp"

#import "TestObj.h"

TEST_CASE( "OCTest/TestObj", "tests TestObj" )
{
    TestObj* obj = [[TestObj alloc] init];
    
    REQUIRE( obj.int_val == 0 );
    
    obj.int_val = 1;
    
    REQUIRE( obj.int_val == 1 );
    
    [obj release];
}
