
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_textflow.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <thread>
#include <atomic>

TEST_CASE( "ThreadAssertionTest",
           "[Multithreading]" ) {
    SECTION( "Basic" ) {
        std::atomic_bool should_stop{false};
        std::thread a([&should_stop] () {
            while (!should_stop) {
                FAIL_CHECK(false);
                CHECK(true);
            }
        });
        std::thread b([&should_stop] () {
            while (!should_stop) {
                FAIL_CHECK(false);
                CHECK(true);
            }
        });
        std::this_thread::sleep_for( std::chrono::milliseconds( 1'000 ) );
        should_stop = true;
        a.join();
        b.join();
    }
}
