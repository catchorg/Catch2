
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_LEAK_DETECTOR_HPP_INCLUDED
#define CATCH_LEAK_DETECTOR_HPP_INCLUDED

#include <catch2/internal/catch_dll_public.hpp>

namespace Catch {

    struct CATCH_DLL_PUBLIC LeakDetector {
        LeakDetector();
        ~LeakDetector();
    };
}
#endif // CATCH_LEAK_DETECTOR_HPP_INCLUDED
