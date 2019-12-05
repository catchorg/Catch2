//
//  TestObj.h
//  OCTest
//
//  Created by Phil on 13/11/2010.
//  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef TWOBLUECUBES_TESTOBJ_H_INCLUDED
#define TWOBLUECUBES_TESTOBJ_H_INCLUDED

#import <Foundation/Foundation.h>


@interface TestObj : NSObject {

    int int_val;
}

-(BOOL) isTrue;
-(BOOL) isFalse;

@property (nonatomic, assign ) int int_val;

@end

#endif // TWOBLUECUBES_TESTOBJ_H_INCLUDED
