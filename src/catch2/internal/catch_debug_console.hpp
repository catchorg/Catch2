
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_DEBUG_CONSOLE_HPP_INCLUDED
#define CATCH_DEBUG_CONSOLE_HPP_INCLUDED

#include <catch2/internal/catch_dll_public.hpp>
#include <string>

namespace Catch {
    void writeToDebugConsole( std::string const& text );
}

#endif // CATCH_DEBUG_CONSOLE_HPP_INCLUDED
