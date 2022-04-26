
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_STREAM_END_STOP_HPP_INCLUDED
#define CATCH_STREAM_END_STOP_HPP_INCLUDED

#include <catch2/internal/catch_dll_public.hpp>
#include <catch2/internal/catch_stringref.hpp>

namespace Catch {

    // Use this in variadic streaming macros to allow
    //    << +StreamEndStop
    // as well as
    //    << stuff +StreamEndStop
    struct CATCH_DLL_PUBLIC StreamEndStop {
        StringRef operator+() const { return StringRef(); }

        template <typename T>
        friend T const& operator+( T const& value, StreamEndStop ) {
            return value;
        }
    };

} // namespace Catch

#endif // CATCH_STREAM_END_STOP_HPP_INCLUDED
