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
    
class AssertionResultBuilder {
public:
    
    AssertionResultBuilder( ResultWas::OfType resultType = ResultWas::Unknown );
    AssertionResultBuilder( const AssertionResultBuilder& other );
    AssertionResultBuilder& operator=(const AssertionResultBuilder& other );

    AssertionResultBuilder& setResultType( ResultWas::OfType result );
    AssertionResultBuilder& setCapturedExpression( const std::string& capturedExpression );
    AssertionResultBuilder& setIsFalse( bool isFalse );
    AssertionResultBuilder& setLineInfo( const SourceLineInfo& lineInfo );
    AssertionResultBuilder& setLhs( const std::string& lhs );
    AssertionResultBuilder& setRhs( const std::string& rhs );
    AssertionResultBuilder& setOp( const std::string& op );
    AssertionResultBuilder& setMacroName( const std::string& macroName );

    template<typename T>
    AssertionResultBuilder& operator << ( const T& value ) {
        m_stream << value;
        return *this;
    }

    std::string reconstructExpression() const;

    AssertionResult build() const;

    // Disable attempts to use || and && in expressions (without parantheses)
    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator || ( const RhsT& );
    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator && ( const RhsT& );

private:
    AssertionResultData m_data;
    struct ExprComponents {
        ExprComponents() : isFalse( false ) {}
        bool isFalse;
        std::string lhs, rhs, op;
    } m_exprComponents;
    std::ostringstream m_stream;
};

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_ASSERTIONRESULT_BUILDER_H_INCLUDED
