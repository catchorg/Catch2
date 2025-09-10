
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_CONSTEXPR_ASSERTS_HPP_INCLUDED
#define CATCH_CONSTEXPR_ASSERTS_HPP_INCLUDED
#include <catch2/internal/catch_decomposer.hpp>
#include <catch2/internal/catch_test_macro_impl.hpp>

namespace Catch {
    template <typename ExpressionType>
    void handleExpression( StringRef macroName,
                           SourceLineInfo const& lineInfo,
                           StringRef capturedExpression,
                           ResultDisposition::Flags resultDisposition,
                           ExpressionType decomposedExpression ) {
        AssertionHandler catchAssertionHandler(
            macroName, lineInfo, capturedExpression, resultDisposition );
        CATCH_TRY {
            catchAssertionHandler.handleExpr( decomposedExpression );
        } CATCH_CATCH_ALL {
            catchAssertionHandler.handleUnexpectedInflightException();
        }
        catchAssertionHandler.complete();
    }
} // namespace Catch

#define INTERNAL_CATCH_CONSTEXPR_ASSERT( macroName, resultDisposition, ... )   \
    do {                                                                       \
        CATCH_INTERNAL_START_WARNINGS_SUPPRESSION                              \
        CATCH_INTERNAL_SUPPRESS_PARENTHESES_WARNINGS                           \
        const auto expr__InternalCatch__ = Catch::Decomposer() <= __VA_ARGS__; \
        CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION                               \
        if ( expr__InternalCatch__.getResult() !=                              \
             Catch::isFalseTest( resultDisposition ) ) {                       \
        } else                                                                 \
            Catch::handleExpression( macroName##_catch_sr,                     \
                                     CATCH_INTERNAL_LINEINFO,                  \
                                     CATCH_INTERNAL_STRINGIFY( __VA_ARGS__ ),  \
                                     resultDisposition,                        \
                                     expr__InternalCatch__ );                  \
    } while ( false )

#endif // CATCH_CONSTEXPR_ASSERTS_HPP_INCLUDED
