
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_ASSERTION_HANDLER_HPP_INCLUDED
#define CATCH_ASSERTION_HANDLER_HPP_INCLUDED

#include <catch2/catch_assertion_info.hpp>
#include <catch2/internal/catch_decomposer.hpp>

#include <string>

namespace Catch {

    class RunContext;

    struct AssertionReaction {
        bool shouldDebugBreak = false;
        bool shouldThrow = false;
        bool shouldSkip = false;
    };

    class AssertionHandler {
        AssertionInfo m_assertionInfo;
        AssertionReaction m_reaction;
        bool m_completed = false;
        // Since all uses are hidden in the .cpp file, we can directly use
        // the final type and avoid going through virtual dispatch, without
        // massive compilation time overhead.
        RunContext& m_resultCapture;

        void finishIncomplete();

    public:
        AssertionHandler
            (   StringRef macroName,
                SourceLineInfo const& lineInfo,
                StringRef capturedExpression,
                ResultDisposition::Flags resultDisposition );
        ~AssertionHandler() {
            // We want the common fast path inlinable, and the virtual
            // dispatch in a function in single TU.
            if ( !m_completed ) { finishIncomplete(); }
        }


        template<typename T>
        constexpr void handleExpr( ExprLhs<T> const& expr ) {
            handleExpr( expr.makeUnaryExpr() );
        }
        void handleExpr( ITransientExpression const& expr );

        void handleMessage(ResultWas::OfType resultType, std::string&& message);

        void handleExceptionThrownAsExpected();
        void handleUnexpectedExceptionNotThrown();
        void handleExceptionNotThrownAsExpected();
        void handleThrowingCallSkipped();

        // Marking this as `noexcept` gives us significant improvement
        // in (optimized) compilation times. Making it `noexcept` changes
        // what happens if the underlying code throws, **but** the only
        // way this can throw is if the reporter itself throws from
        // `assertionEnded`. However, in such case the process is going
        // to abort anyway, because we will enter `assertionEnded` again
        // from the (noexcept) destructor of `AssertionHandler` above.
        // In the future, all reporter event handlers will be marked
        // `noexcept` to make this limitation explicit.
        void handleUnexpectedInflightException() noexcept;

        void complete();

        // query
        auto allowThrows() const -> bool;
    };

    void handleExceptionMatchExpr( AssertionHandler& handler, std::string const& str );

} // namespace Catch

#endif // CATCH_ASSERTION_HANDLER_HPP_INCLUDED
