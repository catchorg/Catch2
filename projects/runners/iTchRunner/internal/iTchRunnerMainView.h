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
#include "internal/catch_context_impl.hpp"
#include "catch.hpp"

#include "iTchRunnerReporter.h"

#import <UIKit/UIKit.h>

@interface iTchRunnerMainView : UIView<iTchRunnerDelegate, UIActionSheetDelegate>
{
    UITextField* appName;
}

-(void) showAlert;

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
        arcSafeRelease( appName );
        appName.textColor = [[UIColor alloc] initWithRed:0.35 green:0.35 blue:1 alpha:1];
        arcSafeRelease( appName.textColor );
        appName.textAlignment = UITextAlignmentCenter; // or NSTextAlignmentCenter
        
        appName.text = [NSString stringWithFormat:@"CATCH tests"];
//        [self performSelector: @selector(showAlert) withObject:nil afterDelay:0.1];
        [self performSelectorOnMainThread:@selector(showAlert) withObject:nil waitUntilDone:NO];
        
    }
    return self;
}

///////////////////////////////////////////////////////////////////////////////
-(void) dealloc
{
    [appName removeFromSuperview];
#if !CATCH_ARC_ENABLED
    [super dealloc];
#endif
}

///////////////////////////////////////////////////////////////////////////////
-(void) showAlert
{    
    UIActionSheet* menu = [[UIActionSheet alloc] initWithTitle:@"Options"
                                                      delegate:self
                                             cancelButtonTitle:nil
                                        destructiveButtonTitle:nil
                                             otherButtonTitles:@"Run all tests", nil];
    [menu showInView: self];
    arcSafeRelease( menu );
 
}

///////////////////////////////////////////////////////////////////////////////
-(void) actionSheet: (UIActionSheet*) sheet clickedButtonAtIndex: (NSInteger) index
{
    Catch::Config config;
    config.setReporter( new Catch::iTchRunnerReporter( self ) );
    Catch::Runner runner( config );
    
    config.getReporter()->StartGroup( "" );
    runner.runAll( true );
    config.getReporter()->EndGroup( "", runner.getTotals() );

    if( runner.getTotals().assertions.failed == 0 )
    {
        NSLog( @"no failures" );
        if( runner.getTotals().assertions.passed > 0 )
            appName.textColor = [[UIColor alloc] initWithRed:0.35 green:1 blue:0.35 alpha:1];
    }
    else
    {
        NSLog( @"%lu failures", runner.getTotals().assertions.failed );
        appName.textColor = [[UIColor alloc] initWithRed:1 green:0.35 blue:0.35 alpha:1];
    }
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
