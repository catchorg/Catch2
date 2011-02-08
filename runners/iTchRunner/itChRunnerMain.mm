//
//  iTchRunnerMain.mm
//  iTchRunner
//
//  Created by Phil on 04/02/2011.
//  Copyright Two Blue Cubes Ltd 2011. All rights reserved.
//

#import "internal/iTchRunnerAppDelegate.h"

int main(int argc, char *argv[]) 
{
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain(argc, argv, nil, @"iTchRunnerAppDelegate");
    [pool release];
    return retVal;
}
