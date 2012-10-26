/*
 *  Created by Phil on 8/5/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_ASSERTIONRESULT_BUILDER_H_INCLUDED
#define TWOBLUECUBES_CATCH_ASSERTIONRESULT_BUILDER_H_INCLUDED

#include "catch_tostring.hpp"
#include "catch_assertionresult.h"
#include "catch_result_type.h"
#include "catch_evaluate.hpp"
#include "catch_common.h"

namespace Catch {
    
class ExpressionResultBuilder {
public:
    
    ExpressionResultBuilder( ResultWas::OfType resultType = ResultWas::Unknown );
    ExpressionResultBuilder( const ExpressionResultBuilder& other );
    ExpressionResultBuilder& operator=(const ExpressionResultBuilder& other );

    ExpressionResultBuilder& setResultType( ResultWas::OfType result );
    ExpressionResultBuilder& setResultType( bool result );
    ExpressionResultBuilder& setLhs( const std::string& lhs );
    ExpressionResultBuilder& setRhs( const std::string& rhs );
    ExpressionResultBuilder& setOp( const std::string& op );

    ExpressionResultBuilder& negate( bool shouldNegate );

    template<typename T>
    ExpressionResultBuilder& operator << ( const T& value ) {
        m_stream << value;
        return *this;
    }

    std::string reconstructExpression( const AssertionInfo& info ) const;

    AssertionResultData build( const AssertionInfo& info ) const;

private:
    AssertionResultData m_data;
    struct ExprComponents {
        ExprComponents() : shouldNegate( false ) {}
        bool shouldNegate;
        std::string lhs, rhs, op;
    } m_exprComponents;
    std::ostringstream m_stream;
};

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_ASSERTIONRESULT_BUILDER_H_INCLUDED
