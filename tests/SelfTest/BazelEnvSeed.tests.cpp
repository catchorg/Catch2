
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <catch2/interfaces/catch_interfaces_config.hpp>
#include <catch2/internal/catch_context.hpp>
#include <cstdlib>
#include <cstdint>

TEST_CASE("Env TEST_RANDOM_SEED sets seed when CLI not provided", "[bazel-seed]") {
    auto const* cfg = Catch::getCurrentContext().getConfig();

    const char* v = std::getenv("TEST_RANDOM_SEED");
    if (!v) {
        
        SUCCEED("TEST_RANDOM_SEED not set; skipping env-specific check");
        return;
    }

    char* end = nullptr;
    unsigned long long parsed = std::strtoull(v, &end, 10);
    REQUIRE(end != v);
    REQUIRE(*end == '\0');

    REQUIRE(cfg->rngSeed() == static_cast<std::uint32_t>(parsed));
}
