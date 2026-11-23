
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <iostream>
#include <cstdio>
#include <cstddef>

namespace regression_3221 {
    struct Value { int value; };
    struct NullValue {};
}

// These deliberately live outside Value's namespace, before Catch2 headers.
#define DEFINE_GLOBAL_COMPARISON(op)                                      \
    constexpr bool operator op(regression_3221::Value const& lhs,          \
                               regression_3221::Value const& rhs) {       \
        return lhs.value op rhs.value;                                    \
    }
DEFINE_GLOBAL_COMPARISON(==)
DEFINE_GLOBAL_COMPARISON(!=)
DEFINE_GLOBAL_COMPARISON(<)
DEFINE_GLOBAL_COMPARISON(<=)
DEFINE_GLOBAL_COMPARISON(>)
DEFINE_GLOBAL_COMPARISON(>=)
#undef DEFINE_GLOBAL_COMPARISON

constexpr bool operator==(regression_3221::NullValue const&, std::nullptr_t) {
    return true;
}
constexpr bool operator==(std::nullptr_t, regression_3221::NullValue const&) {
    return true;
}
constexpr bool operator!=(regression_3221::NullValue const&, std::nullptr_t) {
    return false;
}
constexpr bool operator!=(std::nullptr_t, regression_3221::NullValue const&) {
    return false;
}

namespace {

struct truthy {
    truthy(bool b):m_value(b){}
    operator bool() const {
        return false;
    }
    bool m_value;
};

std::ostream& operator<<(std::ostream& o, truthy) {
    o << "Hey, its truthy!";
    return o;
}

} // end anonymous namespace

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Global comparisons declared before Catch2 headers: #3221",
          "[Decomposition][approvals]") {
    regression_3221::Value const one{1}, two{2};
    REQUIRE(one == one);
    REQUIRE_FALSE(one == two);
    REQUIRE(one != two);
    REQUIRE_FALSE(one != one);
    REQUIRE(one < two);
    REQUIRE(one <= one);
    REQUIRE(two > one);
    REQUIRE(two >= two);
    REQUIRE(regression_3221::Value{1} == regression_3221::Value{1});
    REQUIRE(one != regression_3221::Value{2});
    regression_3221::NullValue const null;
    REQUIRE(null == 0);
    REQUIRE(0 == null);
    REQUIRE(null == 0L);
    REQUIRE(0L == null);
    REQUIRE_FALSE(null != 0);
    REQUIRE_FALSE(0 != null);
    REQUIRE_FALSE(null != 0L);
    REQUIRE_FALSE(0L != null);
}

TEST_CASE( "Reconstruction should be based on stringification: #914" , "[Decomposition][failing][.]") {
    CHECK(truthy(false));
}

TEST_CASE("#1005: Comparing pointer to int and long (NULL can be either on various systems)", "[Decomposition][approvals]") {
    FILE* fptr = nullptr;
    REQUIRE( fptr == 0 );
    REQUIRE_FALSE( fptr != 0 );
    REQUIRE( fptr == 0l );
    REQUIRE_FALSE( fptr != 0l );
}
