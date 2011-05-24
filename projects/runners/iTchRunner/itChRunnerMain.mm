//
//  iTchRunnerMain.mm
//  iTchRunner
//
//  Created by Phil on 04/02/2011.
//  Copyright Two Blue Cubes Ltd 2011. All rights reserved.
//

#include "catch.hpp"
#import "internal/iTchRunnerAppDelegate.h"

int main(int argc, char *argv[]) 
{    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    Catch::registerTestMethods();
    int retVal = UIApplicationMain(argc, argv, nil, @"iTchRunnerAppDelegate");
    [pool release];
    return retVal;
}
