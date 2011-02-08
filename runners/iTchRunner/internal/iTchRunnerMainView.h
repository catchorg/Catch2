/*
 *  iTchRunnerMainView.h
 *  iTchRunner
 *
 *  Created by Phil on 07/02/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 */

#include "internal/catch_config.hpp"
#include "internal/catch_runner_impl.hpp"
#include "internal/catch_hub_impl.hpp"
#include "catch.hpp"

#include "iTchRunnerReporter.h"

#import <UIKit/UIKit.h>

@interface iTchRunnerMainView : UIView<iTchRunnerDelegate, UIActionSheetDelegate>
{
    UITextField* appName;
}

@end

@implementation iTchRunnerMainView


///////////////////////////////////////////////////////////////////////////////
-(id) initWithFrame:(CGRect)frame
{
    if ((self = [super initWithFrame:frame])) 
    {
        // Initialization code
        self.backgroundColor = [UIColor blackColor];
        
        appName = [[UITextField alloc] initWithFrame: CGRectMake( 0, 50, 320, 50 )];
        [self addSubview: appName];
        [appName release];
        appName.textColor = [[UIColor alloc] initWithRed:0.35 green:0.35 blue:1 alpha:1];
        [appName.textColor release];
        appName.textAlignment = UITextAlignmentCenter;
        
        NSString* appNameStr = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleName"];
        if( [appNameStr isEqualToString:@"iTchRunner"] )
            appName.text = @"iTchRunner self-tests";
        else
            appName.text = [NSString stringWithFormat:@"CATCH tests for %@", appNameStr];

    
        UIActionSheet* menu = [[UIActionSheet alloc] initWithTitle:@"Options"
                                                          delegate:self
                                                 cancelButtonTitle:nil
                                            destructiveButtonTitle:nil
                                                 otherButtonTitles:@"Run all tests", nil];
        [menu showInView: self];
        [menu release];
        
    }
    return self;
}

///////////////////////////////////////////////////////////////////////////////
-(void) dealloc
{
    [appName removeFromSuperview];
    [super dealloc];
}

///////////////////////////////////////////////////////////////////////////////
-(void) actionSheet: (UIActionSheet*) sheet clickedButtonAtIndex: (NSInteger) index
{
    Catch::Config config;
    config.setReporter( new Catch::iTchRunnerReporter( self ) );
    Catch::Runner runner( config );
    
    config.getReporter()->StartGroup( "" );
    runner.runAll( true );
    config.getReporter()->EndGroup( "", runner.getSuccessCount(), runner.getFailureCount() );
}

///////////////////////////////////////////////////////////////////////////////
-(void) testWasRun: (const Catch::ResultInfo*) pResultInfo
{
    const Catch::ResultInfo& resultInfo = *pResultInfo;
    std::ostringstream oss;
    
    if( resultInfo.hasExpression() )
    {
        oss << resultInfo.getExpression();
        if( resultInfo.ok() )
            oss << " succeeded";
        else
            oss << " failed";
    }
    switch( resultInfo.getResultType() )
    {
        case Catch::ResultWas::ThrewException:
            if( resultInfo.hasExpression() )
                oss << " with unexpected";
            else
                oss << "Unexpected";
            oss << " exception with message: '" << resultInfo.getMessage() << "'";
            break;
        case Catch::ResultWas::Info:
            oss << "info: '" << resultInfo.getMessage() << "'";
            break;
        case Catch::ResultWas::Warning:
            oss << "warning: '" << resultInfo.getMessage() << "'";
            break;
        case Catch::ResultWas::ExplicitFailure:
            oss << "failed with message: '" << resultInfo.getMessage() << "'";
            break;
        default:
            break;
    }
    
    if( resultInfo.hasExpression() )
    {
        oss << " for: " << resultInfo.getExpandedExpression();
    }
    oss << std::endl;
    NSLog( @"%s", oss.str().c_str() );
}


@end
