
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <iostream>
#include <cstddef>
#include <cstdio>

namespace global_operator_test {
    struct Value {
        int value;
    };

    struct NullComparable {
        bool is_null;
    };
}

constexpr bool operator==( global_operator_test::Value const& lhs,
                           global_operator_test::Value const& rhs ) {
    return lhs.value == rhs.value;
}

constexpr bool operator!=( global_operator_test::Value const& lhs,
                           global_operator_test::Value const& rhs ) {
    return lhs.value != rhs.value;
}

constexpr bool operator<( global_operator_test::Value const& lhs,
                          global_operator_test::Value const& rhs ) {
    return lhs.value < rhs.value;
}

constexpr bool operator<=( global_operator_test::Value const& lhs,
                           global_operator_test::Value const& rhs ) {
    return lhs.value <= rhs.value;
}

constexpr bool operator>( global_operator_test::Value const& lhs,
                          global_operator_test::Value const& rhs ) {
    return lhs.value > rhs.value;
}

constexpr bool operator>=( global_operator_test::Value const& lhs,
                           global_operator_test::Value const& rhs ) {
    return lhs.value >= rhs.value;
}

constexpr bool operator==( global_operator_test::NullComparable const& value,
                           std::nullptr_t ) {
    return value.is_null;
}

constexpr bool operator==( std::nullptr_t,
                           global_operator_test::NullComparable const& value ) {
    return value.is_null;
}

constexpr bool operator!=( global_operator_test::NullComparable const& value,
                           std::nullptr_t ) {
    return !value.is_null;
}

constexpr bool operator!=( std::nullptr_t,
                           global_operator_test::NullComparable const& value ) {
    return !value.is_null;
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

TEST_CASE( "#3221: Global comparison operators participate in decomposition",
           "[Decomposition][approvals]" ) {
    using global_operator_test::Value;
    Value const one{ 1 };
    Value const two{ 2 };

    SECTION( "Lvalue operands" ) {
        REQUIRE( one == one );
        REQUIRE_FALSE( one == two );
        REQUIRE( one != two );
        REQUIRE_FALSE( one != one );
        REQUIRE( one < two );
        REQUIRE_FALSE( two < one );
        REQUIRE( one <= two );
        REQUIRE_FALSE( two <= one );
        REQUIRE( two > one );
        REQUIRE_FALSE( one > two );
        REQUIRE( two >= one );
        REQUIRE_FALSE( one >= two );
    }

    SECTION( "Rvalue left operand" ) {
        REQUIRE( Value{ 1 } == one );
        REQUIRE_FALSE( Value{ 1 } == two );
        REQUIRE( Value{ 1 } != two );
        REQUIRE_FALSE( Value{ 1 } != one );
        REQUIRE( Value{ 1 } < two );
        REQUIRE_FALSE( Value{ 2 } < one );
        REQUIRE( Value{ 1 } <= two );
        REQUIRE_FALSE( Value{ 2 } <= one );
        REQUIRE( Value{ 2 } > one );
        REQUIRE_FALSE( Value{ 1 } > two );
        REQUIRE( Value{ 2 } >= one );
        REQUIRE_FALSE( Value{ 1 } >= two );
    }

    SECTION( "Rvalue right operand" ) {
        REQUIRE( one == Value{ 1 } );
        REQUIRE_FALSE( one == Value{ 2 } );
        REQUIRE( one != Value{ 2 } );
        REQUIRE_FALSE( one != Value{ 1 } );
        REQUIRE( one < Value{ 2 } );
        REQUIRE_FALSE( two < Value{ 1 } );
        REQUIRE( one <= Value{ 2 } );
        REQUIRE_FALSE( two <= Value{ 1 } );
        REQUIRE( two > Value{ 1 } );
        REQUIRE_FALSE( one > Value{ 2 } );
        REQUIRE( two >= Value{ 1 } );
        REQUIRE_FALSE( one >= Value{ 2 } );
    }

    SECTION( "Rvalue operands" ) {
        REQUIRE( Value{ 1 } == Value{ 1 } );
        REQUIRE_FALSE( Value{ 1 } == Value{ 2 } );
        REQUIRE( Value{ 1 } != Value{ 2 } );
        REQUIRE_FALSE( Value{ 1 } != Value{ 1 } );
        REQUIRE( Value{ 1 } < Value{ 2 } );
        REQUIRE_FALSE( Value{ 2 } < Value{ 1 } );
        REQUIRE( Value{ 1 } <= Value{ 2 } );
        REQUIRE_FALSE( Value{ 2 } <= Value{ 1 } );
        REQUIRE( Value{ 2 } > Value{ 1 } );
        REQUIRE_FALSE( Value{ 1 } > Value{ 2 } );
        REQUIRE( Value{ 2 } >= Value{ 1 } );
        REQUIRE_FALSE( Value{ 1 } >= Value{ 2 } );
    }
}

TEST_CASE( "#3221: Global comparison operators preserve literal-zero decomposition",
           "[Decomposition][approvals]" ) {
    global_operator_test::NullComparable const empty{ true };
    global_operator_test::NullComparable const present{ false };

    REQUIRE( empty == nullptr );
    REQUIRE_FALSE( present == nullptr );
    REQUIRE( present != nullptr );
    REQUIRE_FALSE( empty != nullptr );
    REQUIRE( nullptr == empty );
    REQUIRE_FALSE( nullptr == present );
    REQUIRE( nullptr != present );
    REQUIRE_FALSE( nullptr != empty );

    REQUIRE( empty == 0 );
    REQUIRE_FALSE( present == 0 );
    REQUIRE( present != 0 );
    REQUIRE_FALSE( empty != 0 );
    REQUIRE( 0 == empty );
    REQUIRE_FALSE( 0 == present );
    REQUIRE( 0 != present );
    REQUIRE_FALSE( 0 != empty );

    REQUIRE( empty == 0L );
    REQUIRE_FALSE( present == 0L );
    REQUIRE( present != 0L );
    REQUIRE_FALSE( empty != 0L );
    REQUIRE( 0L == empty );
    REQUIRE_FALSE( 0L == present );
    REQUIRE( 0L != present );
    REQUIRE_FALSE( 0L != empty );
}
