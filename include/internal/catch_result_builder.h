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
#include "catch_tostring.h"

namespace Catch {

    struct TestFailureException{};

    template<typename T> class ExpressionLhs;

    struct STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison;

    struct CopyableStream {
        CopyableStream() {}
        CopyableStream( CopyableStream const& other ) {
            oss << other.oss.str();
        }
        CopyableStream& operator=( CopyableStream const& other ) {
            oss.str("");
            oss << other.oss.str();
            return *this;
        }
        std::ostringstream oss;
    };

    struct AnyTypeHolderBase {
        virtual std::string toString() = 0;
    };

    template <typename T>
    struct Decay { typedef T type; };

    template <typename T>
    struct Decay<T[]> { typedef T* type; };

    template <typename T, size_t N>
    struct Decay<T[N]> { typedef T* type; };

    template <typename T>
    struct AnyTypeHolder : AnyTypeHolderBase {
        AnyTypeHolder( const T& value ) : value ( value ) {}
        std::string toString() { return Catch::toString( value ); }
        const typename Decay<const T>::type value;
    };

    class ResultBuilder {
    public:
        ResultBuilder(  char const* macroName,
                        SourceLineInfo const& lineInfo,
                        char const* capturedExpression,
                        ResultDisposition::Flags resultDisposition,
                        char const* secondArg = "" );

        template<typename T>
        ExpressionLhs<T const&> operator <= ( T const& operand );
        ExpressionLhs<bool> operator <= ( bool value );

        template<typename T>
        ResultBuilder& operator << ( T const& value ) {
            m_stream.oss << value;
            return *this;
        }

        template<typename RhsT> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator && ( RhsT const& );
        template<typename RhsT> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator || ( RhsT const& );

        ResultBuilder& setResultType( ResultWas::OfType result );
        ResultBuilder& setResultType( bool result );
        template <typename T>
        ResultBuilder& setLhs( T const& lhs ) {
            m_exprComponents.lhs = new AnyTypeHolder<T>( lhs );
            return *this;
        }
        template <typename T>
        ResultBuilder& setRhs( T const& rhs ) {
            m_exprComponents.rhs = new AnyTypeHolder<T>( rhs );
            return *this;
        }
        ResultBuilder& setOp( std::string const& op );

        void endExpression();

        std::string reconstructExpression() const;
        AssertionResult build() const;

        void useActiveException( ResultDisposition::Flags resultDisposition = ResultDisposition::Normal );
        void captureResult( ResultWas::OfType resultType );
        void captureExpression();
        void captureExpectedException( std::string const& expectedMessage );
        void captureExpectedException( Matchers::Impl::Matcher<std::string> const& matcher );
        void handleResult( AssertionResult const& result );
        void react();
        bool shouldDebugBreak() const;
        bool allowThrows() const;

    private:
        AssertionInfo m_assertionInfo;
        AssertionResultData m_data;
        struct ExprComponents {
            ExprComponents() : testFalse( false ), lhs( NULL ), rhs( NULL ) {}
            ~ExprComponents() { delete lhs; delete rhs;  }
            bool testFalse;
            std::string op;
            AnyTypeHolderBase* lhs, *rhs;
        } m_exprComponents;
        CopyableStream m_stream;

        bool m_shouldDebugBreak;
        bool m_shouldThrow;
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

} // namespace Catch

#endif // TWOBLUECUBES_CATCH_RESULT_BUILDER_H_INCLUDED
