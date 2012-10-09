/*
 *  Created by Phil on 11/5/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_EXPRESSION_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_EXPRESSION_HPP_INCLUDED

#include "catch_resultinfo_builder.h"
#include "catch_evaluate.hpp"

namespace Catch {

template<typename T>
class Expression {
	void operator = ( const Expression& );

public:
    Expression( ResultInfoBuilder& result, T lhs )
    :   m_result( result.setLhs( Catch::toString( lhs ) ) ),
        m_lhs( lhs )
    {}
    
    template<typename RhsT>
    ResultInfoBuilder& operator == ( const RhsT& rhs ) {
        return captureExpression<Internal::IsEqualTo>( rhs );
    }

    template<typename RhsT>
    ResultInfoBuilder& operator != ( const RhsT& rhs ) {
        return captureExpression<Internal::IsNotEqualTo>( rhs );
    }
    
    template<typename RhsT>
    ResultInfoBuilder& operator < ( const RhsT& rhs ) {
        return captureExpression<Internal::IsLessThan>( rhs );
    }
    
    template<typename RhsT>
    ResultInfoBuilder& operator > ( const RhsT& rhs ) {
        return captureExpression<Internal::IsGreaterThan>( rhs );
    }
    
    template<typename RhsT>
    ResultInfoBuilder& operator <= ( const RhsT& rhs ) {
        return captureExpression<Internal::IsLessThanOrEqualTo>( rhs );
    }
    
    template<typename RhsT>
    ResultInfoBuilder& operator >= ( const RhsT& rhs ) {
        return captureExpression<Internal::IsGreaterThanOrEqualTo>( rhs );
    }

    ResultInfoBuilder& operator == ( bool rhs ) {
        return captureExpression<Internal::IsEqualTo>( rhs );
    }
    
    ResultInfoBuilder& operator != ( bool rhs ) {
        return captureExpression<Internal::IsNotEqualTo>( rhs );
    }
    
    operator ResultInfoBuilder& () {
        return m_result.setResultType( m_lhs ? ResultWas::Ok : ResultWas::ExpressionFailed );
    }
    
    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator + ( const RhsT& );
    
    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator - ( const RhsT& );

private:
    template<Internal::Operator Op, typename RhsT>
    ResultInfoBuilder& captureExpression( const RhsT& rhs ) {
        return m_result
            .setResultType( Internal::compare<Op>( m_lhs, rhs ) ? ResultWas::Ok : ResultWas::ExpressionFailed )
            .setRhs( Catch::toString( rhs ) )
            .setOp( Internal::OperatorTraits<Op>::getName() );
    }

private:
    ResultInfoBuilder& m_result;
    T m_lhs;
};

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_EXPRESSION_HPP_INCLUDED
