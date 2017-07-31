/*
 *  Created by Phil on 09/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"
#include <iostream>

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#endif

TEST_CASE( "INFO and WARN do not abort tests", "[messages][.]" )
{
    INFO( "this is a " << "message" );    // This should output the message if a failure occurs
    WARN( "this is a " << "warning" );    // This should always output the message but then continue
}
TEST_CASE( "SUCCEED counts as a test pass", "[messages]" )
{
    SUCCEED( "this is a " << "success" );
}

TEST_CASE( "INFO gets logged on failure", "[failing][messages][.]" )
{
    INFO( "this message should be logged" );
    INFO( "so should this" );
    int a = 2;
    REQUIRE( a == 1 );
}

TEST_CASE( "INFO gets logged on failure, even if captured before successful assertions", "[failing][messages][.]" )
{
    INFO( "this message may be logged later" );
    int a = 2;
    CHECK( a == 2 );

    INFO( "this message should be logged" );

    CHECK( a == 1 );

    INFO( "and this, but later" );

    CHECK( a == 0 );

    INFO( "but not this" );

    CHECK( a == 2 );
}

TEST_CASE( "FAIL aborts the test", "[failing][messages][.]" )
{
    FAIL( "This is a " << "failure" );    // This should output the message and abort
    WARN( "We should never see this");
}

TEST_CASE( "FAIL_CHECK does not abort the test", "[failing][messages][.]" )
{
    FAIL_CHECK( "This is a " << "failure" );    // This should output the message then continue
    WARN( "This message appears in the output");
}

#ifdef CATCH_CONFIG_VARIADIC_MACROS
TEST_CASE( "FAIL does not require an argument", "[failing][messages][.]" )
{
    FAIL();
}
TEST_CASE( "SUCCESS does not require an argument", "[messages][.]" )
{
   SUCCEED();
}
#endif

TEST_CASE( "Output from all sections is reported", "[failing][messages][.]" )
{
    SECTION( "one", "" )
    {
        FAIL( "Message from section one" );
    }

    SECTION( "two", "" )
    {
        FAIL( "Message from section two" );
    }
}

TEST_CASE( "Standard output from all sections is reported", "[messages][.]" )
{
    SECTION( "one", "" )
    {
        std::cout << "Message from section one" << std::endl;
    }

    SECTION( "two", "" )
    {
        std::cout << "Message from section two" << std::endl;
    }
}

TEST_CASE( "SCOPED_INFO is reset for each loop", "[messages][failing][.]" )
{
    for( int i=0; i<100; i++ )
    {
        SCOPED_INFO( "current counter " << i );
        SCOPED_CAPTURE( i );
        REQUIRE( i < 10 );
    }
}

TEST_CASE( "The NO_FAIL macro reports a failure but does not fail the test", "[messages]" )
{
    CHECK_NOFAIL( 1 == 2 );
}

TEST_CASE( "just info", "[info][isolated info][messages]" )
{
    INFO( "this should never be seen" );
}
TEST_CASE( "just failure", "[fail][isolated info][.][messages]" )
{
    FAIL( "Previous info should not be seen" );
}


TEST_CASE( "sends information to INFO", "[.][failing]" )
{
    INFO( "hi" );
    int i = 7;
    CAPTURE( i );
    REQUIRE( false );
}

TEST_CASE( "Pointers can be converted to strings", "[messages][.]" )
{
    int p;
    WARN( "actual address of p: " << &p );
    WARN( "toString(p): " << Catch::toString( &p ) );
}
