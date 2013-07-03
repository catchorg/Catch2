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

struct STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison;

// Wraps the (stringised versions of) the lhs, operator and rhs of an expression - as well as
// the result of evaluating it. This is used to build an AssertionResult object
class ExpressionResultBuilder {
public:

    ExpressionResultBuilder( ResultWas::OfType resultType = ResultWas::Unknown );
    ExpressionResultBuilder( ExpressionResultBuilder const& other );
    ExpressionResultBuilder& operator=(ExpressionResultBuilder const& other );

    ExpressionResultBuilder& setResultType( ResultWas::OfType result );
    ExpressionResultBuilder& setResultType( bool result );
    ExpressionResultBuilder& setLhs( std::string const& lhs );
    ExpressionResultBuilder& setRhs( std::string const& rhs );
    ExpressionResultBuilder& setOp( std::string const& op );

    ExpressionResultBuilder& endExpression( ResultDisposition::Flags resultDisposition );

    template<typename T>
    ExpressionResultBuilder& operator << ( T const& value ) {
        m_stream << value;
        return *this;
    }

    std::string reconstructExpression( AssertionInfo const& info ) const;

    AssertionResult buildResult( AssertionInfo const& info ) const;

    template<typename RhsT> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator && ( RhsT const& );
    template<typename RhsT> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator || ( RhsT const& );

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
