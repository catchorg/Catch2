/*
 *  Created by Martin on 06/10/2019.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

#include "internal/catch_random_number_generator.h"

TEST_CASE("Our PCG implementation provides expected results for known seeds", "[rng]") {
    Catch::SimplePcg32 rng;
    SECTION("Default seeded") {
        REQUIRE(rng() == 0xfcdb943b);
        REQUIRE(rng() == 0x6f55b921);
        REQUIRE(rng() == 0x4c17a916);
        REQUIRE(rng() == 0x71eae25f);
        REQUIRE(rng() == 0x6ce7909c);
    }
    SECTION("Specific seed") {
        rng.seed(0xabcd1234);
        REQUIRE(rng() == 0x57c08495);
        REQUIRE(rng() == 0x33c956ac);
        REQUIRE(rng() == 0x2206fd76);
        REQUIRE(rng() == 0x3501a35b);
        REQUIRE(rng() == 0xfdffb30f);

        // Also check repeated output after reseeding
        rng.seed(0xabcd1234);
        REQUIRE(rng() == 0x57c08495);
        REQUIRE(rng() == 0x33c956ac);
        REQUIRE(rng() == 0x2206fd76);
        REQUIRE(rng() == 0x3501a35b);
        REQUIRE(rng() == 0xfdffb30f);
    }
}

TEST_CASE("Comparison ops", "[rng]") {
    using Catch::SimplePcg32;
    REQUIRE(SimplePcg32{} == SimplePcg32{});
    REQUIRE(SimplePcg32{ 0 } != SimplePcg32{});
    REQUIRE_FALSE(SimplePcg32{ 1 } == SimplePcg32{ 2 });
    REQUIRE_FALSE(SimplePcg32{ 1 } != SimplePcg32{ 1 });
}
