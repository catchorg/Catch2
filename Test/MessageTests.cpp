/*
 *  MessageTests.cpp
 *  Catch - Test
 *
 *  Created by Phil on 09/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "../catch.hpp"

TEST_CASE( "succeeding/message", "INFO and WARN do not abort tests" )
{
    INFO( "this is a " << "message" );    // This should output the message but continue
    WARN( "this is a " << "warning" );    // This should output the message but continue
}

TEST_CASE( "failing/message", "FAIL aborts the test" )
{
    FAIL( "This is a " << "failure" );    // This should output the message and abort
}