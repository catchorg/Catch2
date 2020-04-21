/*
 *  Created by Phil on 8/8/2017.
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_assertionhandler.h"
#include "catch_assertionresult.h"
#include "catch_interfaces_runner.h"
#include "catch_interfaces_config.h"
#include "catch_context.h"
#include "catch_debugger.h"
#include "catch_interfaces_registry_hub.h"
#include "catch_capture_matchers.h"
#include "catch_run_context.h"
#include "catch_enforce.h"

namespace Catch {

    namespace {
        auto operator <<( std::ostream& os, ITransientExpression const& expr ) -> std::ostream& {
            expr.streamReconstructedExpression( os );
            return os;
        }
    }

    LazyExpression::LazyExpression( bool isNegated )
    :   m_isNegated( isNegated )
    {}

    LazyExpression::LazyExpression( LazyExpression const& other ) : m_isNegated( other.m_isNegated ) {}

    LazyExpression::operator bool() const {
        return m_transientExpression != nullptr;
    }

    auto operator << ( std::ostream& os, LazyExpression const& lazyExpr ) -> std::ostream& {
        if( lazyExpr.m_isNegated )
            os << "!";

        if( lazyExpr ) {
            if( lazyExpr.m_isNegated && lazyExpr.m_transientExpression->isBinaryExpression() )
                os << "(" << *lazyExpr.m_transientExpression << ")";
            else
                os << *lazyExpr.m_transientExpression;
        }
        else {
            os << "{** error - unchecked empty expression requested **}";
        }
        return os;
    }

    AssertionHandler::AssertionHandler
        (   StringRef const& macroName,
            SourceLineInfo const& lineInfo,
            StringRef capturedExpression,
            ResultDisposition::Flags resultDisposition )
    :   m_assertionInfo{ macroName, lineInfo, capturedExpression, resultDisposition },
        m_resultCapture( getResultCapture() )
    {}

    void AssertionHandler::handleExpr( ITransientExpression const& expr ) {
        m_resultCapture.handleExpr( m_assertionInfo, expr, m_reaction );
    }
    void AssertionHandler::handleMessage(ResultWas::OfType resultType, StringRef const& message) {
        m_resultCapture.handleMessage( m_assertionInfo, resultType, message, m_reaction );
    }

    auto AssertionHandler::allowThrows() const -> bool {
        return getCurrentContext().getConfig()->allowThrows();
    }

    void AssertionHandler::complete() {
        setCompleted();
        if( m_reaction.shouldDebugBreak ) {

            // If you find your debugger stopping you here then go one level up on the
            // call-stack for the code that caused it (typically a failed assertion)

            // (To go back to the test and change execution, jump over the throw, next)
            CATCH_BREAK_INTO_DEBUGGER();
        }
        if (m_reaction.shouldThrow) {
#if !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
            throw Catch::TestFailureException();
#else
            CATCH_ERROR( "Test failure requires aborting test!" );
#endif
        }
    }
    void AssertionHandler::setCompleted() {
        m_completed = true;
    }

    void AssertionHandler::handleUnexpectedInflightException() {
        m_resultCapture.handleUnexpectedInflightException( m_assertionInfo, Catch::translateActiveException(), m_reaction );
    }

    void AssertionHandler::handleExceptionThrownAsExpected() {
        m_resultCapture.handleNonExpr(m_assertionInfo, ResultWas::Ok, m_reaction);
    }
    void AssertionHandler::handleExceptionNotThrownAsExpected() {
        m_resultCapture.handleNonExpr(m_assertionInfo, ResultWas::Ok, m_reaction);
    }

    void AssertionHandler::handleUnexpectedExceptionNotThrown() {
        m_resultCapture.handleUnexpectedExceptionNotThrown( m_assertionInfo, m_reaction );
    }

    void AssertionHandler::handleThrowingCallSkipped() {
        m_resultCapture.handleNonExpr(m_assertionInfo, ResultWas::Ok, m_reaction);
    }

    // This is the overload that takes a string and infers the Equals matcher from it
    // The more general overload, that takes any string matcher, is in catch_capture_matchers.cpp
    void handleExceptionMatchExpr( AssertionHandler& handler, std::string const& str, StringRef const& matcherString  ) {
        handleExceptionMatchExpr( handler, Matchers::Equals( str ), matcherString );
    }

} // namespace Catch
