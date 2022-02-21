
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>

#include <catch2/internal/catch_stream.hpp>

TEST_CASE( "Cout stream properly declares it writes to stdout", "[streams]" ) {
    REQUIRE( Catch::makeStream( "-" )->isStdout() );
}

TEST_CASE( "Empty stream name opens cout stream", "[streams]" ) {
    REQUIRE( Catch::makeStream( "" )->isStdout() );
}
