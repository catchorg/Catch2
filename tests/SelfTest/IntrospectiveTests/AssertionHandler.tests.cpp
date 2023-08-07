
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "Incomplete AssertionHandler", "[assertion-handler][!shouldfail]" ) {
    Catch::AssertionHandler catchAssertionHandler(
        "REQUIRE"_catch_sr,
        CATCH_INTERNAL_LINEINFO,
        "Dummy",
        Catch::ResultDisposition::Normal );
}
