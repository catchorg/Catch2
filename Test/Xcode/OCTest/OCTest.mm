/*
 *  OCTest.mm
 *  OCTest
 *
 *  Created by Phil on 13/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 */

#import "../../../catch.hpp"

#import "TestObj.h"

TEST_CASE( "OCTest/TestObj", "tests TestObj" )
{
    TestObj* obj = [[TestObj alloc] init];
    
    EXPECT( obj.int_val == 0 );
    
    obj.int_val = 1;
    
    EXPECT( obj.int_val == 1 );
    
    [obj release];
}
