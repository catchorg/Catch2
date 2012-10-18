/*
 *  Created by Phil on 11/5/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_EXPRESSION_BUILDER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_EXPRESSION_BUILDER_HPP_INCLUDED

#include "catch_expression.hpp"
#include "catch_assertionresult_builder.h"

namespace Catch {
    
class ExpressionBuilder {
public:

    ExpressionBuilder( bool isFalse = false )
    {
        m_result.setIsFalse( isFalse );
    }
    
    template<typename T>
    Expression<const T&> operator->* ( const T & operand ) {
        Expression<const T&> expr( m_result, operand );        
        return expr;
    }

    Expression<bool> operator->* ( bool value ) {
        Expression<bool> expr( m_result, value );
        return expr;
    }

private:
    AssertionResultBuilder m_result;
};

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_EXPRESSION_BUILDER_HPP_INCLUDED
