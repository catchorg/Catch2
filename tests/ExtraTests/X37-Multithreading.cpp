
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_textflow.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <thread>
#include <stop_token>

TEST_CASE( "ThreadAssertionTest",
           "[Multithreading]" ) {
    SECTION( "Basic" ) {
        std::jthread a([] (const std::stop_token& token) {
            while (!token.stop_requested()) {
                FAIL_CHECK(false);
                CHECK(true);
            }
        });
        std::jthread b([] (const std::stop_token& token) {
            while (!token.stop_requested()) {
                FAIL_CHECK(false);
                CHECK(true);
            }
        });
        std::this_thread::sleep_for( std::chrono::milliseconds( 1'000 ) );
        a.get_stop_source().request_stop();
        b.get_stop_source().request_stop();
    }
}
