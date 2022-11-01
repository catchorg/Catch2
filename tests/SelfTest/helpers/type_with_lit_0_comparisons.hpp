
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#ifndef CATCH_TEST_HELPERS_TYPE_WITH_LIT_0_COMPARISONS_HPP_INCLUDED
#define CATCH_TEST_HELPERS_TYPE_WITH_LIT_0_COMPARISONS_HPP_INCLUDED

#include <type_traits>

// Should only be constructible from literal 0.
// Used by `TypeWithLit0Comparisons` for testing comparison
// ops that only work with literal zero, the way std::*orderings do
struct ZeroLiteralDetector {
    constexpr ZeroLiteralDetector( ZeroLiteralDetector* ) noexcept {}

    template <typename T,
              typename = std::enable_if_t<!std::is_same<T, int>::value>>
    constexpr ZeroLiteralDetector( T ) = delete;
};

struct TypeWithLit0Comparisons {
#define DEFINE_COMP_OP( op )                                                  \
    friend bool operator op( TypeWithLit0Comparisons, ZeroLiteralDetector ) { \
        return true;                                                          \
    }                                                                         \
    friend bool operator op( ZeroLiteralDetector, TypeWithLit0Comparisons ) { \
        return false;                                                         \
    }

    DEFINE_COMP_OP( < )
    DEFINE_COMP_OP( <= )
    DEFINE_COMP_OP( > )
    DEFINE_COMP_OP( >= )
    DEFINE_COMP_OP( == )
    DEFINE_COMP_OP( != )

#undef DEFINE_COMP_OP
};

#endif // CATCH_TEST_HELPERS_TYPE_WITH_LIT_0_COMPARISONS_HPP_INCLUDED
