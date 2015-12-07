/*
 *  Created by Phil on 24/11/2015.
 *  Copyright 2015 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

#include "../include/internal/catch_suppress_warnings.h"
#include "../include/internal/catch_thread_context.hpp"

#define THREADED_SECTION SECTION

TEST_CASE( "multithreaded sections" ) {


    THREADED_SECTION( "test" ) {
        Catch::ThreadContext tctx( Catch::getGlobalRunContext() );

    }
}
