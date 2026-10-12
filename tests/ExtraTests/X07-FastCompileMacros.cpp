
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

/**\file
 * Test that various basic macros work with CATCH_CONFIG_FAST_COMPILE.
 *
 * Note that the current checking is rather loose. We check that the
 * macros compile, and that the test cases (don't) fail as they are
 * supposed to.
 */

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>

namespace {

    [[noreturn]]
    static void throws() {
        throw std::runtime_error{ "sup" };
    }
    static void doesnt_throw() {}
    [[noreturn]]
    static int throws_i() {
        throw std::runtime_error{ "sup" };
    }

} // namespace

TEST_CASE( "Passing macros work" ) {
    REQUIRE( 1 != 2 );
    CHECK( 2 == 2 );
    REQUIRE_THROWS( throws() );
    REQUIRE_NOTHROW( doesnt_throw() );
}

TEST_CASE( "Failing macros work", "[!shouldfail]" ) {
    CHECK( 1 != 2 );
    CHECK( 2 == 2 );
    CHECK( 3 == 2 );
}

TEST_CASE( "Failing NOTHROW works", "[!shouldfail]" ) {
    REQUIRE_NOTHROW( throws() );
}

TEST_CASE( "Failing THROW works", "[!shouldfail]" ) {
    REQUIRE_THROWS( doesnt_throw() );
}

TEST_CASE( "Unexpected exception in REQUIRE gets inverted properly",
           "[!shouldfail]" ) {
    REQUIRE( throws_i() == 1 );
}

TEST_CASE( "Unexpected exception in REQUIRE fails properly" ) {
    REQUIRE( throws_i() == 2 );
}
