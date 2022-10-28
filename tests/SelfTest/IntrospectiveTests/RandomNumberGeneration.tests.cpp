
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_random_number_generator.hpp>
#include <catch2/internal/catch_random_seed_generation.hpp>
#include <catch2/generators/catch_generators.hpp>

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

TEST_CASE("Random seed generation reports unknown methods", "[rng][seed]") {
    REQUIRE_THROWS(Catch::generateRandomSeed(static_cast<Catch::GenerateFrom>(77)));
}

TEST_CASE("Random seed generation accepts known methods", "[rng][seed]") {
    using Catch::GenerateFrom;
    const auto method = GENERATE(
        GenerateFrom::Time,
        GenerateFrom::RandomDevice,
        GenerateFrom::Default
    );

    REQUIRE_NOTHROW(Catch::generateRandomSeed(method));
}
