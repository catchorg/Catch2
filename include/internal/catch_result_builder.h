/*
 *  Created by Phil on 28/5/2014.
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RESULT_BUILDER_H_INCLUDED
#define TWOBLUECUBES_CATCH_RESULT_BUILDER_H_INCLUDED

#include "catch_result_type.h"
#include "catch_assertionresult.h"
#include "catch_common.h"
#include "catch_matchers.hpp"

namespace Catch {

    struct TestFailureException{};

    template<typename T> class ExpressionLhs;

    struct CopyableStream {
        CopyableStream() {}
        CopyableStream( CopyableStream const& other ) {
            oss << other.oss.str();
        }
        CopyableStream& operator=( CopyableStream const& other ) {
            oss.str(std::string());
            oss << other.oss.str();
            return *this;
        }
        std::ostringstream oss;
    };

    class ResultBuilder : public DecomposedExpression {
    public:
        ResultBuilder(  char const* macroName,
                        SourceLineInfo const& lineInfo,
                        char const* capturedExpression,
                        ResultDisposition::Flags resultDisposition,
                        char const* secondArg = "" );
        ~ResultBuilder();

        template<typename T>
        ExpressionLhs<T const&> operator <= ( T const& operand );
        ExpressionLhs<bool> operator <= ( bool value );

        template<typename T>
        ResultBuilder& operator << ( T const& value ) {
            m_stream().oss << value;
            return *this;
        }

        ResultBuilder& setResultType( ResultWas::OfType result );
        ResultBuilder& setResultType( bool result );

        void endExpression( DecomposedExpression const& expr );

        virtual void reconstructExpression( std::string& dest ) const CATCH_OVERRIDE;

        AssertionResult build() const;
        AssertionResult build( DecomposedExpression const& expr ) const;

        void useActiveException( ResultDisposition::Flags resultDisposition = ResultDisposition::Normal );
        void captureResult( ResultWas::OfType resultType );
        void captureExpression();
        void captureExpectedException( std::string const& expectedMessage );
        void captureExpectedException( Matchers::Impl::MatcherBase<std::string> const& matcher );
        void handleResult( AssertionResult const& result );
        void react();
        bool shouldDebugBreak() const;
        bool allowThrows() const;

        template<typename ArgT, typename MatcherT>
        void captureMatch( ArgT const& arg, MatcherT const& matcher, char const* matcherString );

        void setExceptionGuard();
        void unsetExceptionGuard();

    private:
        AssertionInfo m_assertionInfo;
        AssertionResultData m_data;

        static CopyableStream &m_stream()
        {
            static CopyableStream s;
            return s;
        }

        bool m_shouldDebugBreak;
        bool m_shouldThrow;
        bool m_guardException;
    };

} // namespace Catch

// Include after due to circular dependency:
#include "catch_expression_lhs.hpp"

namespace Catch {

    template<typename T>
    inline ExpressionLhs<T const&> ResultBuilder::operator <= ( T const& operand ) {
        return ExpressionLhs<T const&>( *this, operand );
    }

    inline ExpressionLhs<bool> ResultBuilder::operator <= ( bool value ) {
        return ExpressionLhs<bool>( *this, value );
    }

    template<typename ArgT, typename MatcherT>
    inline void ResultBuilder::captureMatch( ArgT const& arg, MatcherT const& matcher,
                                             char const* matcherString ) {
        MatchExpression<ArgT const&, MatcherT const&> expr( arg, matcher, matcherString );
        setResultType( matcher.match( arg ) );
        endExpression( expr );
    }


} // namespace Catch

#endif // TWOBLUECUBES_CATCH_RESULT_BUILDER_H_INCLUDED
