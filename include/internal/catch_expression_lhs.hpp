/*
 *  Created by Phil on 11/5/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_EXPRESSION_LHS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_EXPRESSION_LHS_HPP_INCLUDED

#include "catch_result_builder.h"
#include "catch_evaluate.hpp"
#include "catch_tostring.h"

namespace Catch {

// Wraps the LHS of an expression and captures the operator and RHS (if any) -
// wrapping them all in a ResultBuilder object
template<typename T>
class ExpressionLhs {
    ExpressionLhs& operator = ( ExpressionLhs const& );
#  ifdef CATCH_CONFIG_CPP11_GENERATED_METHODS
    ExpressionLhs& operator = ( ExpressionLhs && ) = delete;
#  endif

public:
    ExpressionLhs( ResultBuilder& rb, T lhs ) : m_rb( rb ), m_lhs( lhs ) {}
#  ifdef CATCH_CONFIG_CPP11_GENERATED_METHODS
    ExpressionLhs( ExpressionLhs const& ) = default;
    ExpressionLhs( ExpressionLhs && )     = default;
#  endif

    template<typename RhsT>
    ResultBuilder& operator == ( RhsT const& rhs ) {
        return captureExpression<Internal::IsEqualTo>( rhs );
    }

    template<typename RhsT>
    ResultBuilder& operator != ( RhsT const& rhs ) {
        return captureExpression<Internal::IsNotEqualTo>( rhs );
    }

    template<typename RhsT>
    ResultBuilder& operator < ( RhsT const& rhs ) {
        return captureExpression<Internal::IsLessThan>( rhs );
    }

    template<typename RhsT>
    ResultBuilder& operator > ( RhsT const& rhs ) {
        return captureExpression<Internal::IsGreaterThan>( rhs );
    }

    template<typename RhsT>
    ResultBuilder& operator <= ( RhsT const& rhs ) {
        return captureExpression<Internal::IsLessThanOrEqualTo>( rhs );
    }

    template<typename RhsT>
    ResultBuilder& operator >= ( RhsT const& rhs ) {
        return captureExpression<Internal::IsGreaterThanOrEqualTo>( rhs );
    }

    ResultBuilder& operator == ( bool rhs ) {
        return captureExpression<Internal::IsEqualTo>( rhs );
    }

    ResultBuilder& operator != ( bool rhs ) {
        return captureExpression<Internal::IsNotEqualTo>( rhs );
    }

    void endExpression() {
        bool value = m_lhs ? true : false;
        m_rb
            .setLhs( Catch::toString( value ) )
            .setResultType( value )
            .endExpression();
    }

    // Only simple binary expressions are allowed on the LHS.
    // If more complex compositions are required then place the sub expression in parentheses
    template<typename RhsT> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator + ( RhsT const& );
    template<typename RhsT> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator - ( RhsT const& );
    template<typename RhsT> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator / ( RhsT const& );
    template<typename RhsT> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator * ( RhsT const& );
    template<typename RhsT> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator && ( RhsT const& );
    template<typename RhsT> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator || ( RhsT const& );

private:
    template<Internal::Operator Op, typename RhsT>
    ResultBuilder& captureExpression( RhsT const& rhs ) {
        return m_rb
            .setResultType( Internal::compare<Op>( m_lhs, rhs ) )
            .setLhs( Catch::toString( m_lhs ) )
            .setRhs( Catch::toString( rhs ) )
            .setOp( Internal::OperatorTraits<Op>::getName() );
    }

private:
    ResultBuilder& m_rb;
    T m_lhs;
};

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_EXPRESSION_LHS_HPP_INCLUDED
