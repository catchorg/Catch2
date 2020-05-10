/*
 *  Created by Phil on 8/8/2017.
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_ASSERTIONHANDLER_H_INCLUDED
#define TWOBLUECUBES_CATCH_ASSERTIONHANDLER_H_INCLUDED

#include <catch2/catch_assertion_info.hpp>
#include <catch2/internal/catch_decomposer.hpp>
#include <catch2/interfaces/catch_interfaces_capture.hpp>
#include <catch2/internal/catch_lazy_expr.hpp>

namespace Catch {

    struct TestFailureException{};
    struct AssertionResultData;
    struct IResultCapture;
    class RunContext;

    struct AssertionReaction {
        bool shouldDebugBreak = false;
        bool shouldThrow = false;
    };

    class AssertionHandler {
        AssertionInfo m_assertionInfo;
        AssertionReaction m_reaction;
        bool m_completed = false;
        IResultCapture& m_resultCapture;

    public:
        AssertionHandler
            (   StringRef const& macroName,
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

        void handleMessage(ResultWas::OfType resultType, StringRef const& message);

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

    void handleExceptionMatchExpr( AssertionHandler& handler, std::string const& str, StringRef const& matcherString );

} // namespace Catch

#endif // TWOBLUECUBES_CATCH_ASSERTIONHANDLER_H_INCLUDED
