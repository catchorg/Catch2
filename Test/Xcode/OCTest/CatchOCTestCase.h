//
//  CatchOCTestCase.h
//  OCTest
//
//  Created by Phil on 13/11/2010.
//  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
//

#include "../../../catch_objc.hpp"

#import <Cocoa/Cocoa.h>
#import "TestObj.h"

@interface TestFixture : NSObject <OcFixture>
{
    TestObj* obj;
}

@end