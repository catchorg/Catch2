/*
 *  Created by Phil on 11/5/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_EXPRESSION_DECOMPOSER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_EXPRESSION_DECOMPOSER_HPP_INCLUDED

#include "catch_expression_lhs.hpp"

namespace Catch {

// Captures the LHS of the expression and wraps it in an Expression Lhs object
class ExpressionDecomposer {
public:

    template<typename T>
    ExpressionLhs<const T&> operator->* ( const T & operand ) {
        return ExpressionLhs<const T&>( operand );
    }

    ExpressionLhs<bool> operator->* ( bool value ) {
        return ExpressionLhs<bool>( value );
    }
};

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_EXPRESSION_DECOMPOSER_HPP_INCLUDED
