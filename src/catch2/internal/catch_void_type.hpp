
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_VOID_TYPE_HPP_INCLUDED
#define CATCH_VOID_TYPE_HPP_INCLUDED

#include <catch2/internal/catch_dll_public.hpp>

namespace Catch {
    namespace Detail {

        template <typename...> struct CATCH_DLL_PUBLIC make_void {
            using type = void;
        };

        template <typename... Ts>
        using void_t = typename make_void<Ts...>::type;

    } // namespace Detail
} // namespace Catch


#endif // CATCH_VOID_TYPE_HPP_INCLUDED
