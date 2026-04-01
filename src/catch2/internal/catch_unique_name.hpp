
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_UNIQUE_NAME_HPP_INCLUDED
#define CATCH_UNIQUE_NAME_HPP_INCLUDED

#include <catch2/internal/catch_compiler_capabilities.hpp>
#include <catch2/internal/catch_config_counter.hpp>

// Fixme: Clang 22 has an annoying bug where the localized suppression
//        below does not actually suppress the extension warning from
//        using __COUNTER__, so we have to leak the suppression for the
//        whole TU. Hopefully Clang 23 fixes this before full release.
//        As AppleClang does its own thing version-wise, we ignore it
//        completely.
#if defined( __clang__ ) && ( __clang_major__ >= 22 ) && !defined( __APPLE__ )
CATCH_INTERNAL_SUPPRESS_COUNTER_WARNINGS
#endif

#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) name##line
#define INTERNAL_CATCH_UNIQUE_NAME_LINE( name, line ) \
    INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line )

#ifdef CATCH_CONFIG_COUNTER
#    define INTERNAL_CATCH_UNIQUE_NAME( name )               \
        CATCH_INTERNAL_START_WARNINGS_SUPPRESSION            \
        CATCH_INTERNAL_SUPPRESS_COUNTER_WARNINGS             \
        INTERNAL_CATCH_UNIQUE_NAME_LINE( name, __COUNTER__ ) \
        CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION

#else
#    define INTERNAL_CATCH_UNIQUE_NAME( name ) \
        INTERNAL_CATCH_UNIQUE_NAME_LINE( name, __LINE__ )
#endif

#endif // CATCH_UNIQUE_NAME_HPP_INCLUDED
