
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_CONSTEXPR_SECTION_HPP_INCLUDED
#define CATCH_CONSTEXPR_SECTION_HPP_INCLUDED

#if defined( CATCH_CPP17_OR_GREATER ) && __cpp_constexpr >= 201603
#    define CATCH_CONFIG_CONSTEXPR_SECTIONS
#endif

#include <catch2/internal/catch_compiler_capabilities.hpp>
#include <catch2/internal/catch_section.hpp>
#include <catch2/internal/catch_unique_name.hpp>

namespace Catch {

    struct ConstexprSection {
        template <typename Callable>
        ConstexprSection( Callable callable ) {
            static_assert(
                [&] // error: CONSTEXPR_SECTION failure. Check the compiler
                    // output to find the cause. You can make the section a
                    // SECTION to investigate at runtime.
                {
                    callable();
                    return true;
                }(),
                "compile time section error" );
            addFakeAssertion( "unknown-file", 0 );
            callable();
        }

        // Add a fake assertion to the section to avoid failures because the
        // section is empty
        static void addFakeAssertion( const char* file, size_t lineNumber );
    };
} // namespace Catch

#ifdef CATCH_CONFIG_CONSTEXPR_SECTIONS

#    define INTERNAL_CATCH_CONSTEXPR_SECTION( ... )                          \
        INTERNAL_CATCH_SECTION( "[Passed during compilation] " __VA_ARGS__ ) \
        [[maybe_unused]] const ::Catch::ConstexprSection                     \
        INTERNAL_CATCH_UNIQUE_NAME(                                          \
            catch_internal_CompileTimeSection ) = [&]()

#else
#    define INTERNAL_CATCH_CONSTEXPR_SECTION( ... )                        \
        static_assert( false, "C++17 is required for CONSTEXPR_SECTION" ); \
        if ( false ) {                                                     \
        } else
#endif

#endif // CATCH_CONSTEXPR_SECTION_HPP_INCLUDED
