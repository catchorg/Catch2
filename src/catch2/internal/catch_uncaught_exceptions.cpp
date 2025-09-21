
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/internal/catch_uncaught_exceptions.hpp>
#include <catch2/catch_user_config.hpp>

#include <exception>

namespace Catch {
    bool uncaught_exceptions() {
#if defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
        return false;
#else
        return std::uncaught_exceptions() > 0;
#endif
  }
} // end namespace Catch
