/*
 *  Created by Phil on 09/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

TEST_CASE( "./succeeding/message", "INFO and WARN do not abort tests" )
{
    INFO( "this is a " << "message" );    // This should output the message if a failure occurs
    WARN( "this is a " << "warning" );    // This should always output the message but then continue
}

TEST_CASE( "./failing/message/info/1", "INFO gets logged on failure" )
{
    INFO( "this message should be logged" );
    INFO( "so should this" );
    int a = 2;
    REQUIRE( a == 1 );
}

TEST_CASE( "./mixed/message/info/2", "INFO gets logged on failure" )
{
    INFO( "this message should be logged" );
    int a = 2;
    CHECK( a == 2 );

    INFO( "this message should be logged, too" );
    
    CHECK( a == 1 );

    INFO( "and this, but later" );

    CHECK( a == 0 );

    INFO( "but not this" );

    CHECK( a == 2 );
}

TEST_CASE( "./failing/message/fail", "FAIL aborts the test" )
{
    FAIL( "This is a " << "failure" );    // This should output the message and abort
}

TEST_CASE( "./failing/message/sections", "Output from all sections is reported" )
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

TEST_CASE( "./succeeding/message/sections/stdout", "Output from all sections is reported" )
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

TEST_CASE( "./mixed/message/scoped", "" )
{
    for( int i=0; i<100; i++ )
    {
       SCOPED_INFO( "current counter " << i );
       REQUIRE( i < 10 );
    }
}
