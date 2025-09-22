
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_STREAM_END_STOP_HPP_INCLUDED
#define CATCH_STREAM_END_STOP_HPP_INCLUDED

#include <string_view>

namespace Catch {

    // Use this in variadic streaming macros to allow
    //    << +StreamEndStop
    // as well as
    //    << stuff +StreamEndStop
    struct StreamEndStop {
        constexpr std::string_view operator+() const { return std::string_view(); }

        template <typename T>
        constexpr friend T const& operator+( T const& value, StreamEndStop ) {
            return value;
        }
    };

} // namespace Catch

#endif // CATCH_STREAM_END_STOP_HPP_INCLUDED
