
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/internal/catch_random_seed_generation.hpp>

#include <catch2/internal/catch_enforce.hpp>

#include <ctime>
#include <random>

namespace Catch {

    std::uint32_t generateRandomSeed( GenerateFrom from ) {
        switch ( from ) {
        case GenerateFrom::Time:
            return static_cast<std::uint32_t>( std::time( nullptr ) );

        case GenerateFrom::Default:
        case GenerateFrom::RandomDevice:
            // In theory, a platform could have random_device that returns just
            // 16 bits. That is still some randomness, so we don't care too much
            return static_cast<std::uint32_t>( std::random_device{}() );

        default:
            CATCH_ERROR("Unknown generation method");
        }
    }

} // end namespace Catch
