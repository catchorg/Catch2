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
    :   m_result( result ),
        m_lhs( lhs )
    {}
    
    template<typename RhsT>
    ResultInfoBuilder& operator == ( const RhsT& rhs ) {
        return captureExpression<Internal::IsEqualTo>( m_result, m_lhs, rhs );
    }

    template<typename RhsT>
    ResultInfoBuilder& operator != ( const RhsT& rhs ) {
        return captureExpression<Internal::IsNotEqualTo>( m_result, m_lhs, rhs );
    }
    
    template<typename RhsT>
    ResultInfoBuilder& operator < ( const RhsT& rhs ) {
        return captureExpression<Internal::IsLessThan>( m_result, m_lhs, rhs );
    }
    
    template<typename RhsT>
    ResultInfoBuilder& operator > ( const RhsT& rhs ) {
        return captureExpression<Internal::IsGreaterThan>( m_result, m_lhs, rhs );
    }
    
    template<typename RhsT>
    ResultInfoBuilder& operator <= ( const RhsT& rhs ) {
        return captureExpression<Internal::IsLessThanOrEqualTo>( m_result, m_lhs, rhs );
    }
    
    template<typename RhsT>
    ResultInfoBuilder& operator >= ( const RhsT& rhs ) {
        return captureExpression<Internal::IsGreaterThanOrEqualTo>( m_result, m_lhs, rhs );
    }

    ResultInfoBuilder& operator == ( bool rhs ) {
        return captureExpression<Internal::IsEqualTo>( m_result, m_lhs, rhs );
    }
    
    ResultInfoBuilder& operator != ( bool rhs ) {
        return captureExpression<Internal::IsNotEqualTo>( m_result, m_lhs, rhs );
    }
    
    operator ResultInfoBuilder& () {
        return captureBoolExpression( m_result, m_lhs );
    }
    
    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator + ( const RhsT& );
    
    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator - ( const RhsT& );

private:
    ResultInfoBuilder& m_result;
    T m_lhs;
};

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_EXPRESSION_HPP_INCLUDED
