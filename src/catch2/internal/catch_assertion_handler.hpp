
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_ASSERTION_HANDLER_HPP_INCLUDED
#define CATCH_ASSERTION_HANDLER_HPP_INCLUDED

#include <catch2/catch_assertion_info.hpp>
#include <catch2/interfaces/catch_interfaces_capture.hpp>
#include <catch2/internal/catch_decomposer.hpp>
#include <catch2/internal/catch_dll_public.hpp>
#include <catch2/internal/catch_lazy_expr.hpp>

#include <string>

namespace Catch {

    class IResultCapture;

    struct CATCH_DLL_PUBLIC AssertionReaction {
        bool shouldDebugBreak = false;
        bool shouldThrow = false;
    };

    class CATCH_DLL_PUBLIC AssertionHandler {
        AssertionInfo m_assertionInfo;
        AssertionReaction m_reaction;
        bool m_completed = false;
        IResultCapture& m_resultCapture;

    public:
        AssertionHandler
            (   StringRef macroName,
                SourceLineInfo const& lineInfo,
                StringRef capturedExpression,
                ResultDisposition::Flags resultDisposition );
        ~AssertionHandler() {
            if ( !m_completed ) {
                m_resultCapture.handleIncomplete( m_assertionInfo );
            }
        }


        template<typename T>
        void handleExpr( ExprLhs<T> const& expr ) {
            handleExpr( expr.makeUnaryExpr() );
        }
        void handleExpr( ITransientExpression const& expr );

        void handleMessage(ResultWas::OfType resultType, StringRef message);

        void handleExceptionThrownAsExpected();
        void handleUnexpectedExceptionNotThrown();
        void handleExceptionNotThrownAsExpected();
        void handleThrowingCallSkipped();
        void handleUnexpectedInflightException();

        void complete();
        void setCompleted();

        // query
        auto allowThrows() const -> bool;
    };

    CATCH_DLL_PUBLIC void handleExceptionMatchExpr( AssertionHandler& handler,
                                                    std::string const& str,
                                                    StringRef matcherString );

} // namespace Catch

#endif // CATCH_ASSERTION_HANDLER_HPP_INCLUDED
