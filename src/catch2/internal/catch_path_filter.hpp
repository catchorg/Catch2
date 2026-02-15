
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_PATH_FILTER_HPP_INCLUDED
#define CATCH_PATH_FILTER_HPP_INCLUDED

#include <catch2/internal/catch_move_and_forward.hpp>

#include <string>

namespace Catch {

    struct PathFilter {
        enum class For {
            Section,
            Generator,
        };
        PathFilter( For type_, std::string filter_ ):
            type( type_ ), filter( CATCH_MOVE( filter_ ) ) {}

        For type;
        std::string filter;

        friend bool operator==( PathFilter const& lhs, PathFilter const& rhs );
    };

} // end namespace Catch

#endif // CATCH_PATH_FILTER_HPP_INCLUDED
