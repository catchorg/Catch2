//
//  CatchOCTestCase.mm
//  OCTest
//
//  Created by Phil Nash on 13/11/2010.
//  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
//

#import "CatchOCTestCase.h"


@implementation TestFixture


-(void) setUp
{
    obj = [[TestObj alloc] init];    
}

-(void) tearDown
{
    [obj release];
}

OC_TEST_CASE( "OCTest/test1", "This is a test case" )
{
    REQUIRE( obj.int_val == 0 );
    
    obj.int_val = 1;
    
    REQUIRE( obj.int_val == 1 );
}

OC_TEST_CASE( "OCTest/test2", "This is another test case" )
{
    REQUIRE( obj.int_val == 0 );
    
    obj.int_val = 2;
    
    REQUIRE( obj.int_val == 2 );
}

@end
