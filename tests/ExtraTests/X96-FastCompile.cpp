//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0



/**\file
 * Test that Catch2 works correctly when CATCH_CONFIG_FAST_COMPILE
 * is defined.
 *
 * This flag trades off some features for faster compilation.
 */

 #include <catch2/catch_test_macros.hpp>

TEST_CASE("Fast compile mode runs basic assertions correctly") {
    REQUIRE(1 == 1);
    CHECK(2 == 2);
    REQUIRE_FALSE(1 == 2);
    CHECK_FALSE(3 == 4);

    SECTION("Sections still work") {
        REQUIRE("hello" != "world");
    }
}