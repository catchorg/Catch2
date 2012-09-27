//
//  CatchOCTestCase.h
//  OCTest
//
//  Created by Phil on 13/11/2010.
//  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef TWOBLUECUBES_CATCHOCTESTCASE_H_INCLUDED
#define TWOBLUECUBES_CATCHOCTESTCASE_H_INCLUDED

#include "catch.hpp"

#import <Cocoa/Cocoa.h>
#import "TestObj.h"

@interface TestFixture : NSObject <OcFixture>
{
    TestObj* obj;
}

@end

#endif // TWOBLUECUBES_CATCHOCTESTCASE_H_INCLUDED
