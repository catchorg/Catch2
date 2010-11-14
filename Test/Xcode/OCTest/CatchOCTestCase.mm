//
//  CatchOCTestCase.mm
//  OCTest
//
//  Created by Phil Nash on 13/11/2010.
//  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
//

#import "CatchOCTestCase.h"


@implementation TestFixture


- (id) init
{
    self = [super init];
    if (self != nil) 
    {
        obj = [[TestObj alloc] init];
    }
    return self;
}

- (void) dealloc
{
    [obj release];
    [super dealloc];
}

OC_TEST_CASE( "OCTest/test1", "This is a test case" )
{
    EXPECT( obj.int_val == 0 );
    
    obj.int_val = 1;
    
    EXPECT( obj.int_val == 1 );
}

OC_TEST_CASE( "OCTest/test2", "This is another test case" )
{
    EXPECT( obj.int_val == 0 );
    
    obj.int_val = 2;
    
    EXPECT( obj.int_val == 2 );
}

@end
