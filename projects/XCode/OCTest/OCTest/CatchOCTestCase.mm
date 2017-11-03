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

OC_TEST_CASE( "This is a test case", "" )
{
    REQUIRE( obj.int_val == 0 );

    obj.int_val = 1;

    REQUIRE( obj.int_val == 1 );
}

OC_TEST_CASE( "This is another test case", "" )
{
    REQUIRE( obj.int_val == 0 );

    obj.int_val = 2;

    REQUIRE( obj.int_val == 2 );
}

OC_TEST_CASE( "tests a boolean value", "[!shouldfail]" )
{
    CHECK( [obj isTrue] == NO );
    CHECK( [obj isFalse] == YES );
}

OC_TEST_CASE( "throws an Objective-C exception", "[!throws][!shouldfail]" )
{
    @throw [[NSException alloc] initWithName: NSGenericException
                                      reason: @"Objective-C exception"
                                    userInfo: nil];
}
OC_TEST_CASE( "throws a std c++ exception", "[!throws][!shouldfail]" )
{
    throw std::domain_error( "std C++ exception" );
}

///////////////////////////////////////////////////////////////////////////
template<typename T>
void useObject( const T& object ){}

template<typename T>
void useObject( const T* object ){}

OC_TEST_CASE( "Matches work with OC types (NSString so far)", "[!shouldfail]" )
{
    using namespace Catch::Matchers;

    REQUIRE_THAT( @"This is a string", Equals( @"This isnt a string" ) );
    REQUIRE_THAT( @"This is a string", Contains( @"is a" ) );
    REQUIRE_THAT( @"This is a string", StartsWith( @"This" ) );
    REQUIRE_THAT( @"This is a string", EndsWith( @"string" ) );
}

OC_TEST_CASE( "nil NSString should not crash the test", "[!shouldfail]" )
{
    using namespace Catch::Matchers;

    CHECK_THAT( (NSString*)nil, Equals( @"This should fail, but not crash" ) );
    CHECK_THAT( (NSString*)nil, StartsWith( @"anything" ) );
}

@end
