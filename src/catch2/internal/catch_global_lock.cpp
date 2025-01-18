
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_global_lock.hpp>
#include <catch2/internal/catch_compiler_capabilities.hpp>

CATCH_INTERNAL_START_WARNINGS_SUPPRESSION
CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS

namespace Catch {

    std::recursive_mutex& get_global_lock() {
        static std::recursive_mutex global_lock;
        return global_lock;
    }

} // namespace Catch

CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION
