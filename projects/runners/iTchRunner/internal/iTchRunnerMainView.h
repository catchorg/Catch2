/*
 *  iTchRunnerMainView.h
 *  iTchRunner
 *
 *  Created by Phil on 07/02/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 */
#ifndef TWOBLUECUBES_ITCHRUNNERMAINVIEW_H_INCLUDED
#define TWOBLUECUBES_ITCHRUNNERMAINVIEW_H_INCLUDED

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

// This is a copy & paste from Catch::Runner2 to get us bootstrapped (this is due to all be
// replaced anyway)
inline Catch::Totals runTestsForGroup( Catch::Runner& context, const Catch::TestCaseFilters& filterGroup ) {
    using namespace Catch;
    Totals totals;
    std::vector<TestCase>::const_iterator it = getRegistryHub().getTestCaseRegistry().getAllTests().begin();
    std::vector<TestCase>::const_iterator itEnd = getRegistryHub().getTestCaseRegistry().getAllTests().end();
    int testsRunForGroup = 0;
    for(; it != itEnd; ++it ) {
        if( filterGroup.shouldInclude( *it ) ) {
            testsRunForGroup++;

            if( context.aborting() )
                break;

            totals += context.runTest( *it );
        }
    }
    if( testsRunForGroup == 0 )
        std::cerr << "\n[No test cases matched with: " << filterGroup.getName() << "]" << std::endl;
    return totals;

}

///////////////////////////////////////////////////////////////////////////////
-(void) actionSheet: (UIActionSheet*) sheet clickedButtonAtIndex: (NSInteger) index
{
    Catch::Config config;
    Catch::IReporter* reporter = new Catch::iTchRunnerReporter( self );
    Catch::LegacyReporterAdapter* reporterAdapter = new Catch::LegacyReporterAdapter( reporter, Catch::ReporterConfig( config.stream(), config.data() ) );
    Catch::Runner runner( config, reporterAdapter );


    std::vector<Catch::TestCaseFilters> filterGroups;
    Catch::TestCaseFilters filterGroup( "" );
    filterGroups.push_back( filterGroup );

    Catch::Totals totals;

    std::vector<Catch::TestCaseFilters>::const_iterator it = filterGroups.begin();
    std::vector<Catch::TestCaseFilters>::const_iterator itEnd = filterGroups.end();

    for(; it != itEnd && !runner.aborting(); ++it ) {
        runner.testGroupStarting( it->getName() );
        totals += runTestsForGroup( runner, *it );
        runner.testGroupEnded( it->getName(), totals );
    }


    if( totals.assertions.failed == 0 )
    {
        NSLog( @"no failures" );
        if( totals.assertions.passed > 0 )
            appName.textColor = [[UIColor alloc] initWithRed:0.35 green:1 blue:0.35 alpha:1];
    }
    else
    {
        NSLog( @"%lu failures", totals.assertions.failed );
        appName.textColor = [[UIColor alloc] initWithRed:1 green:0.35 blue:0.35 alpha:1];
    }
}

///////////////////////////////////////////////////////////////////////////////
-(void) testWasRun: (const Catch::AssertionResult*) pResultInfo
{
    const Catch::AssertionResult& resultInfo = *pResultInfo;
    std::ostringstream oss;
    
    if( resultInfo.hasExpression() )
    {
        oss << resultInfo.getExpression();
        if( resultInfo.isOk() )
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

#endif // TWOBLUECUBES_ITCHRUNNERMAINVIEW_H_INCLUDED
