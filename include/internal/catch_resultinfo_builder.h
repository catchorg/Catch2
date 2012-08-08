/*
 *  Created by Phil on 8/5/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RESULTINFO_BUILDER_H_INCLUDED
#define TWOBLUECUBES_CATCH_RESULTINFO_BUILDER_H_INCLUDED

#include "catch_tostring.hpp"
#include "catch_resultinfo.h"
#include "catch_result_type.h"
#include "catch_evaluate.hpp"
#include "catch_common.h"

namespace Catch {

struct STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison;
    
class ResultInfoBuilder : public ResultInfo {

public:
    
    ResultInfoBuilder();
    
    ResultInfoBuilder(  const char* expr, 
                        bool isNot, 
                        const SourceLineInfo& lineInfo,
                        const char* macroName,
                        const char* message = "" );

    void setResultType( ResultWas::OfType result );
    void setMessage( const std::string& message );
    void setLineInfo( const SourceLineInfo& lineInfo );
    void setLhs( const std::string& lhs );
    void setRhs( const std::string& rhs );
    void setOp( const std::string& op );
    
    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator ||
    (
        const RhsT&
    );

    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator &&
    (
        const RhsT&
    );
    
private:
    friend class ExpressionBuilder;
    template<typename T> friend class Expression;

    template<typename T> friend class PtrExpression;

    ResultInfoBuilder& captureBoolExpression( bool result );

    template<Internal::Operator Op, typename T1, typename T2>    
    ResultInfoBuilder& captureExpression( const T1& lhs, const T2& rhs ) {
        setResultType( Internal::compare<Op>( lhs, rhs ) ? ResultWas::Ok : ResultWas::ExpressionFailed );
        m_lhs = Catch::toString( lhs );
        m_rhs = Catch::toString( rhs );
        m_op = Internal::OperatorTraits<Op>::getName();
        return *this;
    }

    template<Internal::Operator Op, typename T>
    ResultInfoBuilder& captureExpression( const T* lhs, int rhs ) {
        return captureExpression<Op>( lhs, reinterpret_cast<const T*>( rhs ) );
    }    
};

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RESULTINFO_BUILDER_H_INCLUDED
