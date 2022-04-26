
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_NONCOPYABLE_HPP_INCLUDED
#define CATCH_NONCOPYABLE_HPP_INCLUDED

#include <catch2/internal/catch_dll_public.hpp>

namespace Catch {
    namespace Detail {

        //! Deriving classes become noncopyable and nonmovable
        class CATCH_DLL_PUBLIC NonCopyable {
            NonCopyable( NonCopyable const& ) = delete;
            NonCopyable( NonCopyable&& ) = delete;
            NonCopyable& operator=( NonCopyable const& ) = delete;
            NonCopyable& operator=( NonCopyable&& ) = delete;

        protected:
            NonCopyable() noexcept = default;
        };

    } // namespace Detail
} // namespace Catch

#endif // CATCH_NONCOPYABLE_HPP_INCLUDED
