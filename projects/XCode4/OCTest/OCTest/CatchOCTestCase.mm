//
//  CatchOCTestCase.mm
//  OCTest
//
//  Created by Phil Nash on 13/11/2010.
//  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#import "CatchOCTestCase.h"


@implementation TestFixture


-(void) setUp
{
    obj = [[TestObj alloc] init];    
}

-(void) tearDown
{
    arcSafeRelease( obj );
}

OC_TEST_CASE( "OCTest/test1", "This is a test case" )
{
    REQUIRE( obj.int_val == 0 );
    
    obj.int_val = 1;
    
    REQUIRE( obj.int_val == 1 );
}

OC_TEST_CASE( "OCTest/test2", "This is another test case" )
{
    REQUIRE( obj.int_val == 0 );
    
    obj.int_val = 2;
    
    REQUIRE( obj.int_val == 2 );
}

OC_TEST_CASE( "OCTest/BOOL", "tests a boolean value" )
{
    CHECK( [obj isTrue] == NO );
    CHECK( [obj isFalse] == YES );
}

OC_TEST_CASE( "OCTest/throws/objc", "throws an Objective-C exception" )
{
    @throw [[NSException alloc] initWithName: NSGenericException
                                      reason: @"Objective-C exception"
                                    userInfo: nil];
}
OC_TEST_CASE( "OCTest/throws/stdc++", "throws a std c++ exception" )
{
    throw std::domain_error( "std C++ exception" );
}

///////////////////////////////////////////////////////////////////////////
template<typename T>
void useObject( const T& object ){}

template<typename T>
void useObject( const T* object ){}

OC_TEST_CASE( "OCTest/matchers", "Matches work with OC types (NSString so far)" )
{
    REQUIRE_THAT( @"This is a string", Equals( @"This isnt a string" ) );
    REQUIRE_THAT( @"This is a string", Contains( @"is a" ) );
    REQUIRE_THAT( @"This is a string", StartsWith( @"This" ) );
    REQUIRE_THAT( @"This is a string", EndsWith( @"string" ) );
}

OC_TEST_CASE( "OCTest/matchers/nil", "nil NSString should not crash the test" )
{
    CHECK_THAT( (NSString*)nil, Equals( @"This should fail, but not crash" ) );
    CHECK_THAT( (NSString*)nil, StartsWith( @"anything" ) );
}

@end
