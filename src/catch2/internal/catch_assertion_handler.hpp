
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

        // TODO:
        //   * called from catch (...) blocks in assertions
        //   * doesn't propagate exception
        //   * Allows the compiler to skip adding more exception edges in catch
        //   (...) when it cannot throw
        //   * compilation is faster
        //   * abort on throw inside is fine -> this is called when another
        //   exception is called, so it would abort anyway
        void handleUnexpectedInflightException() noexcept;
        // TODO:
        //   * similar reasoning as above, unified comment for both
        void handleExceptionThrownAsExpected() noexcept;

        void handleUnexpectedExceptionNotThrown();
        void handleExceptionNotThrownAsExpected();
        void handleThrowingCallSkipped();

        void complete();

        // query
        auto allowThrows() const -> bool;
    };

    // Like the two handlers above, this is only ever called from inside the
    // `catch` block of an assertion macro, so an exception escaping it would
    // already terminate. Saying so explicitly keeps the compiler from emitting
    // a nested exception edge at every expansion of `REQUIRE_THROWS_WITH` and
    // friends.
    void handleExceptionMatchExpr( AssertionHandler& handler, std::string const& str ) noexcept;

} // namespace Catch

#endif // CATCH_ASSERTION_HANDLER_HPP_INCLUDED
