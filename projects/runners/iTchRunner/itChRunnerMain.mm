//
//  iTchRunnerMain.mm
//  iTchRunner
//
//  Created by Phil on 04/02/2011.
//  Copyright Two Blue Cubes Ltd 2011. All rights reserved.
//

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#import "internal/iTchRunnerAppDelegate.h"

int main(int argc, char *argv[]) 
{    
#if !CATCH_ARC_ENABLED
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
#endif
    
    Catch::registerTestMethods();
    int retVal = UIApplicationMain(argc, argv, nil, @"iTchRunnerAppDelegate");

#if !CATCH_ARC_ENABLED
    [pool release];
#endif

    return retVal;
}
