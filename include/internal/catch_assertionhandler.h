/*
 *  Created by Phil on 8/8/2017.
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_ASSERTIONHANDLER_H_INCLUDED
#define TWOBLUECUBES_CATCH_ASSERTIONHANDLER_H_INCLUDED

#include "catch_decomposer.h"
#include "catch_assertioninfo.h"

namespace Catch {

    struct TestFailureException{};
    struct AssertionResultData;

    class LazyExpression {
        friend class AssertionHandler;
        friend struct AssertionStats;

        ITransientExpression const* m_transientExpression = nullptr;
        bool m_isNegated;
    public:
        LazyExpression( bool isNegated );
        LazyExpression( LazyExpression const& other );
        LazyExpression& operator = ( LazyExpression const& ) = delete;

        explicit operator bool() const;

        friend auto operator << ( std::ostream& os, LazyExpression const& lazyExpr ) -> std::ostream&;
    };

    class AssertionHandler {
        AssertionInfo m_assertionInfo;
        bool m_shouldDebugBreak = false;
        bool m_shouldThrow = false;
        bool m_inExceptionGuard = false;

    public:
        AssertionHandler
            (   StringRef macroName,
                SourceLineInfo const& lineInfo,
                StringRef capturedExpression,
                ResultDisposition::Flags resultDisposition );
        ~AssertionHandler();

        void handle( ITransientExpression const& expr );

        template<typename T>
        void handle( ExprLhs<T> const& expr ) {
            handle( expr.makeUnaryExpr() );
        }
        void handle( ResultWas::OfType resultType );
        void handle( ResultWas::OfType resultType, StringRef const& message );
        void handle( ResultWas::OfType resultType, ITransientExpression const* expr, bool negated );
        void handle( AssertionResultData const& resultData, ITransientExpression const* expr );

        auto shouldDebugBreak() const -> bool;
        auto allowThrows() const -> bool;
        void reactWithDebugBreak() const;
        void reactWithoutDebugBreak() const;
        void useActiveException();
        void setExceptionGuard();
        void unsetExceptionGuard();
    };

    void handleExceptionMatchExpr( AssertionHandler& handler, std::string const& str, StringRef matcherString );

} // namespace Catch

#endif // TWOBLUECUBES_CATCH_ASSERTIONHANDLER_H_INCLUDED
