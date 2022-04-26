
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_TEST_FAILURE_EXCEPTION_HPP_INCLUDED
#define CATCH_TEST_FAILURE_EXCEPTION_HPP_INCLUDED

#include <catch2/internal/catch_dll_public.hpp>

namespace Catch {

    //! Used to signal that an assertion macro failed
    struct CATCH_DLL_PUBLIC TestFailureException {};

} // namespace Catch

#endif // CATCH_TEST_FAILURE_EXCEPTION_HPP_INCLUDED
