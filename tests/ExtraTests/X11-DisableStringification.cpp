
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// X11-DisableStringification.cpp
// Test that stringification of original expression can be disabled
// this is a workaround for VS 2017 issue with Raw String literal
// and preprocessor token pasting. In other words, hopefully this test
// will be deleted soon :-)

#include <catch2/catch_test_macros.hpp>

namespace {
    struct Hidden {};

    bool operator==(Hidden, Hidden) { return true; }
}

TEST_CASE("DisableStringification") {
    REQUIRE( Hidden{} == Hidden{} );
}
