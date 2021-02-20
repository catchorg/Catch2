/*
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <thread>

TEST_CASE( "sleep_for_100ms", "[.min_duration_test][approvals]" )
{
  std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
  CHECK( true );
}

TEST_CASE( "sleep_for_1000ms", "[.min_duration_test][approvals]" )
{
  std::this_thread::sleep_for( std::chrono::milliseconds( 1'000 ) );
  CHECK( true );
}
