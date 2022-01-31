
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE( "tests can be skipped dynamically at runtime", "[skipping]" ) {
    SKIP();
    FAIL( "this is not reached" );
}

TEST_CASE( "skipped tests can optionally provide a reason", "[skipping]" ) {
    const int answer = 43;
    SKIP( "skipping because answer = " << answer );
    FAIL( "this is not reached" );
}

TEST_CASE( "sections can be skipped dynamically at runtime", "[skipping]" ) {
    SECTION( "not skipped" ) { SUCCEED(); }
    SECTION( "skipped" ) { SKIP(); }
}

TEST_CASE( "dynamic skipping works with generators", "[skipping]" ) {
    const int answer = GENERATE( 41, 42, 43 );
    if ( answer != 42 ) { SKIP( "skipping because answer = " << answer ); }
    SUCCEED();
}

TEST_CASE( "failed assertions before SKIP are still reported",
           "[skipping][!shouldfail]" ) {
    CHECK( 3 == 4 );
    SKIP();
}
