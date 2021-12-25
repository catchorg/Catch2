
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_CASE_INSENSITIVE_COMPARISONS_HPP_INCLUDED
#define CATCH_CASE_INSENSITIVE_COMPARISONS_HPP_INCLUDED

#include <string>

namespace Catch {
    namespace Detail {
        //! Provides case-insensitive `op<` semantics when called
        struct CaseInsensitiveLess {
            bool operator()( std::string const& lhs,
                             std::string const& rhs ) const;
        };

    } // namespace Detail
} // namespace Catch

#endif // CATCH_CASE_INSENSITIVE_COMPARISONS_HPP_INCLUDED