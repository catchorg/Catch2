
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_COMPARE_TRAITS_HPP_INCLUDED
#define CATCH_COMPARE_TRAITS_HPP_INCLUDED

#include <catch2/internal/catch_void_type.hpp>

#include <type_traits>

namespace Catch {
    namespace Detail {

#if defined( __GNUC__ ) && !defined( __clang__ )
#    pragma GCC diagnostic push
    // GCC likes to complain about comparing bool with 0, in the decltype()
    // that defines the comparable traits below.
#    pragma GCC diagnostic ignored "-Wbool-compare"
    // "ordered comparison of pointer with integer zero" same as above,
    // but it does not have a separate warning flag to suppress
#    pragma GCC diagnostic ignored "-Wextra"
    // Did you know that comparing floats with `0` directly
    // is super-duper dangerous in unevaluated context?
#    pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#if defined( __clang__ )
#    pragma clang diagnostic push
    // Did you know that comparing floats with `0` directly
    // is super-duper dangerous in unevaluated context?
#    pragma clang diagnostic ignored "-Wfloat-equal"
#endif

        // To avoid instantiating all of `is_foo_comparable<T, T&>`,
        // `is_foo_comparable<T&, T&>`, `is_foo_comparable<T, T>`, ...
        // we force the type into simple T ref. This does not cause behaviour
        // change, because the decomposer already only uses lvalues.
        //
        // Note that we do not strip away constness, as that would make
        // the comparability of `const T` dependent on comparability of `T`,
        // which could lead to behavior change from the current implementation.
        template <typename T>
        using IsComparableNormalized_t = std::remove_reference_t<T>&;

#define CATCH_DEFINE_COMPARABLE_TRAIT( id, op )                               \
    template <typename, typename, typename = void>                            \
    struct is_##id##_comparable_impl : std::false_type {};                    \
    template <typename T, typename U>                                         \
    struct is_##id##_comparable_impl<                                         \
        T,                                                                    \
        U,                                                                    \
        void_t<decltype( std::declval<T>() op std::declval<U>() )>>           \
        : std::true_type {};                                                  \
    template <typename T, typename U>                                         \
    using is_##id##_comparable =                                              \
        is_##id##_comparable_impl<IsComparableNormalized_t<T>,                \
                                  IsComparableNormalized_t<U>>;               \
    template <typename, typename = void>                                      \
    struct is_##id##_0_comparable_impl : std::false_type {};                  \
    template <typename T>                                                     \
    struct is_##id##_0_comparable_impl<                                       \
        T,                                                                    \
        void_t<decltype( std::declval<T>() op 0 )>>                           \
        : std::true_type {};                                                  \
    template <typename T>                                                     \
    using is_##id##_0_comparable =                                            \
        is_##id##_0_comparable_impl<IsComparableNormalized_t<T>>;

        // We need all 6 pre-spaceship comparison ops: <, <=, >, >=, ==, !=
        CATCH_DEFINE_COMPARABLE_TRAIT( lt, < )
        CATCH_DEFINE_COMPARABLE_TRAIT( le, <= )
        CATCH_DEFINE_COMPARABLE_TRAIT( gt, > )
        CATCH_DEFINE_COMPARABLE_TRAIT( ge, >= )
        CATCH_DEFINE_COMPARABLE_TRAIT( eq, == )
        CATCH_DEFINE_COMPARABLE_TRAIT( ne, != )

#undef CATCH_DEFINE_COMPARABLE_TRAIT

#if defined( __GNUC__ ) && !defined( __clang__ )
#    pragma GCC diagnostic pop
#endif
#if defined( __clang__ )
#    pragma clang diagnostic pop
#endif


    } // namespace Detail
} // namespace Catch

#endif // CATCH_COMPARE_TRAITS_HPP_INCLUDED
