
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_assertion_handler.hpp>
#include <catch2/interfaces/catch_interfaces_config.hpp>
#include <catch2/internal/catch_context.hpp>
#include <catch2/internal/catch_debugger.hpp>
#include <catch2/internal/catch_test_failure_exception.hpp>
#include <catch2/internal/catch_global_lock.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace Catch {
    // The AssertionHandler API and handleExceptionMatchExpr are used by assertion macros. Everything here must be
    // locked as catch internals are not thread-safe.

    AssertionHandler::AssertionHandler
        (   StringRef macroName,
            SourceLineInfo const& lineInfo,
            StringRef capturedExpression,
            ResultDisposition::Flags resultDisposition )
    :   m_assertionInfo{ macroName, lineInfo, capturedExpression, resultDisposition },
        m_resultCapture( getResultCapture() )
    {
        auto lock = take_global_lock();
        m_resultCapture.notifyAssertionStarted( m_assertionInfo );
    }

    AssertionHandler::~AssertionHandler() {
        auto lock = take_global_lock();
        if ( !m_completed ) {
            m_resultCapture.handleIncomplete( m_assertionInfo );
        }
    }

    void AssertionHandler::handleExpr( ITransientExpression const& expr ) {
        auto lock = take_global_lock();
        m_resultCapture.handleExpr( m_assertionInfo, expr, m_reaction );
    }
    void AssertionHandler::handleMessage(ResultWas::OfType resultType, std::string&& message) {
        auto lock = take_global_lock();
        m_resultCapture.handleMessage( m_assertionInfo, resultType, CATCH_MOVE(message), m_reaction );
    }

    auto AssertionHandler::allowThrows() const -> bool {
        return getCurrentContext().getConfig()->allowThrows();
    }

    void AssertionHandler::complete() {
        m_completed = true;
        if( m_reaction.shouldDebugBreak ) {

            // If you find your debugger stopping you here then go one level up on the
            // call-stack for the code that caused it (typically a failed assertion)

            // (To go back to the test and change execution, jump over the throw, next)
            CATCH_BREAK_INTO_DEBUGGER();
        }
        if (m_reaction.shouldThrow) {
            throw_test_failure_exception();
        }
        if ( m_reaction.shouldSkip ) {
            throw_test_skip_exception();
        }
    }

    void AssertionHandler::handleUnexpectedInflightException() {
        auto lock = take_global_lock();
        m_resultCapture.handleUnexpectedInflightException( m_assertionInfo, Catch::translateActiveException(), m_reaction );
    }

    void AssertionHandler::handleExceptionThrownAsExpected() {
        auto lock = take_global_lock();
        m_resultCapture.handleNonExpr(m_assertionInfo, ResultWas::Ok, m_reaction);
    }
    void AssertionHandler::handleExceptionNotThrownAsExpected() {
        auto lock = take_global_lock();
        m_resultCapture.handleNonExpr(m_assertionInfo, ResultWas::Ok, m_reaction);
    }

    void AssertionHandler::handleUnexpectedExceptionNotThrown() {
        auto lock = take_global_lock();
        m_resultCapture.handleUnexpectedExceptionNotThrown( m_assertionInfo, m_reaction );
    }

    void AssertionHandler::handleThrowingCallSkipped() {
        auto lock = take_global_lock();
        m_resultCapture.handleNonExpr(m_assertionInfo, ResultWas::Ok, m_reaction);
    }

    // This is the overload that takes a string and infers the Equals matcher from it
    // The more general overload, that takes any string matcher, is in catch_capture_matchers.cpp
    void handleExceptionMatchExpr( AssertionHandler& handler, std::string const& str ) {
        handleExceptionMatchExpr( handler, Matchers::Equals( str ) );
    }

} // namespace Catch
