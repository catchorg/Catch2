
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

/**\file
 * Checks that GENERATE over infinite generator errors out when the tests are
 * run with `-warn InfiniteGenerators`
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace {
    static int ONE = 1;
    class infinite_generator : public Catch::Generators::IGenerator<int> {
    public:

        int const& get() const override { return ONE; }
        bool next() override { return true; }
        auto isFinite() const -> bool override { return false; }
    };

    static auto make_infinite_generator()
        -> Catch::Generators::GeneratorWrapper<int> {
        return { new infinite_generator() };
    }

} // namespace

TEST_CASE() {
    auto _ = GENERATE( make_infinite_generator() );
    (void)_;
}

TEST_CASE() {
    REQUIRE(true);
}
