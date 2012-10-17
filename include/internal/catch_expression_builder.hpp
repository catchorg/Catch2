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
#include "catch_tostring.hpp"
#include "catch_assertionresult.h"
#include "catch_result_type.h"
#include "catch_context.h"
#include "catch_common.h"
#include <sstream>

namespace Catch {
    
class ExpressionBuilder {
public:

    ExpressionBuilder( bool isFalse = false )
    : m_messageStream()
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
    
    template<typename T>
    ExpressionBuilder& operator << ( const T & value ) {
        m_messageStream << Catch::toString( value );        
        return *this;
    }
    
    template<typename MatcherT, typename ArgT>
    ExpressionBuilder& acceptMatcher(   const MatcherT& matcher,
                                        const ArgT& arg,
                                        const std::string& matcherCallAsString ) {
        std::string matcherAsString = matcher.toString();
        if( matcherAsString == "{?}" )
            matcherAsString = matcherCallAsString;
        m_result
            .setLhs( Catch::toString( arg ) )
            .setRhs( matcherAsString )
            .setOp( "matches" )
            .setResultType( matcher.match( arg ) ? ResultWas::Ok : ResultWas::ExpressionFailed );
        return *this;
    }
    
    template<typename MatcherT, typename ArgT>
    ExpressionBuilder& acceptMatcher(   const MatcherT& matcher,
                                        ArgT* arg,
                                        const std::string& matcherCallAsString ) {
        std::string matcherAsString = matcher.toString();
        if( matcherAsString == "{?}" )
            matcherAsString = matcherCallAsString;
        m_result
            .setLhs( Catch::toString( arg ) )
            .setRhs( matcherAsString )
            .setOp( "matches" )
            .setResultType( matcher.match( arg ) ? ResultWas::Ok : ResultWas::ExpressionFailed );
        return *this;
    }
    
    ExpressionBuilder& setResultType( ResultWas::OfType resultType ) {
        m_result.setResultType( resultType );
        return *this;
    }
    
    operator AssertionResultBuilder&() {
        m_result.setMessage( m_messageStream.str() );
        return m_result;
    }
    
private:
    AssertionResultBuilder m_result;
    std::ostringstream m_messageStream;
};

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_EXPRESSION_BUILDER_HPP_INCLUDED
