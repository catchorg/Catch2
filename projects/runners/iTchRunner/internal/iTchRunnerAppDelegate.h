/*
 *  iTchRunnerAppDelegate.h
 *  iTchRunner
 *
 *  Created by Phil on 07/02/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 */
#ifndef TWOBLUECUBES_ITCHRUNNERAPPDELEGATE_H_INCLUDED
#define TWOBLUECUBES_ITCHRUNNERAPPDELEGATE_H_INCLUDED

#import "iTchRunnerMainView.h"



@interface iTchRunnerAppDelegate : NSObject <UIApplicationDelegate> 
{
    UIWindow *window;
}

@end


@implementation iTchRunnerAppDelegate


///////////////////////////////////////////////////////////////////////////////
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions 
{        
    window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    [window setUserInteractionEnabled:YES];
    [window setMultipleTouchEnabled:YES];
    
	CGRect screenRect = [[UIScreen mainScreen] applicationFrame];
    iTchRunnerMainView* view = [[iTchRunnerMainView alloc] initWithFrame:screenRect];
    
    [window addSubview:view];
    [window makeKeyAndVisible];
    arcSafeRelease( view );

    return YES;
}

///////////////////////////////////////////////////////////////////////////////
- (void)dealloc 
{
#if !CATCH_ARC_ENABLED
    [window release];
    [super dealloc];
#endif
}


///////////////////////////////////////////////////////////////////////////////
- (void)applicationWillResignActive:(UIApplication *)application 
{
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
}


///////////////////////////////////////////////////////////////////////////////
- (void)applicationDidEnterBackground:(UIApplication *)application 
{
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
}


///////////////////////////////////////////////////////////////////////////////
- (void)applicationWillEnterForeground:(UIApplication *)application 
{
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
}


///////////////////////////////////////////////////////////////////////////////
- (void)applicationDidBecomeActive:(UIApplication *)application 
{
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
}


///////////////////////////////////////////////////////////////////////////////
- (void)applicationWillTerminate:(UIApplication *)application 
{
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
}


///////////////////////////////////////////////////////////////////////////////
- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application 
{
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
}



@end

#endif // TWOBLUECUBES_ITCHRUNNERAPPDELEGATE_H_INCLUDED
