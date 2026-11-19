//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#define CATCH_CONFIG_TEMPLATE_LIST_TEST_CASE_SUFFIX(Type, ListName, Index) \
    throwingSuffix(ListName)
#include <catch2/catch_template_test_macros.hpp>

#include <stdexcept>
#include <tuple>

namespace {
    char const* throwingSuffix( Catch::StringRef listName ) {
        if ( listName == "Types" ) {
            throw std::runtime_error( "Could not name type list test" );
        }
        throw 1;
    }

    template <typename> struct Fixture {};
} // namespace

using Types = std::tuple<int>;
TEMPLATE_LIST_TEST_CASE( "Test with a throwing suffix", "", Types ) {}

using FixtureTypes = std::tuple<int>;
TEMPLATE_LIST_TEST_CASE_METHOD( Fixture, "Fixture with a throwing suffix", "", FixtureTypes ) {}
