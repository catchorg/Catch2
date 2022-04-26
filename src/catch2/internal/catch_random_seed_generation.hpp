
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_RANDOM_SEED_GENERATION_HPP_INCLUDED
#define CATCH_RANDOM_SEED_GENERATION_HPP_INCLUDED

#include <catch2/internal/catch_dll_public.hpp>
#include <cstdint>

namespace Catch {

    enum class CATCH_DLL_PUBLIC GenerateFrom {
        Time,
        RandomDevice,
        //! Currently equivalent to RandomDevice, but can change at any point
        Default
    };

    CATCH_DLL_PUBLIC std::uint32_t generateRandomSeed( GenerateFrom from );

} // end namespace Catch

#endif // CATCH_RANDOM_SEED_GENERATION_HPP_INCLUDED
