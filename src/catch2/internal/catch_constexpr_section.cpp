
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/internal/catch_assertion_handler.hpp>
#include <catch2/internal/catch_constexpr_section.hpp>

namespace Catch {
    void ConstexprSection::addFakeAssertion() {
        AssertionHandler catchAssertionHandler(
            "CONSTEXPR_SECTION",
            SourceLineInfo( "unknown-file", 0 ),
            "[Passed during compilation]",
            ResultDisposition::Normal );
        catchAssertionHandler.handleMessage( ResultWas::Ok,
                                             "[Passed during compilation]" );
        catchAssertionHandler.complete();
    }
} // namespace Catch