/*
 *  Created by Phil on 15/03/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"


TEST_CASE()
{
    SUCCEED( "anonymous test case" );
}

TEST_CASE( "Test case with one argument" )
{
    SUCCEED( "no assertions" );
}

TEST_CASE( "Variadic macros", "[variadic][sections]" )
{
    SECTION( "Section with one argument" )
    {
        SUCCEED( "no assertions" );
    }
}

