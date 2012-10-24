/*
 *  Created by Phil on 8/8/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_ASSERTIONRESULT_BUILDER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_ASSERTIONRESULT_BUILDER_HPP_INCLUDED

#include "catch_assertionresult_builder.h"

#include <assert.h>

namespace Catch {

    AssertionResultBuilder::AssertionResultBuilder( ResultWas::OfType resultType ) {
        m_data.resultType = resultType;
    }
    AssertionResultBuilder::AssertionResultBuilder( const AssertionResultBuilder& other )
    :   m_data( other.m_data ),
        m_exprComponents( other.m_exprComponents )
    {
        m_stream << other.m_stream.str();
    }
    AssertionResultBuilder& AssertionResultBuilder::operator=(const AssertionResultBuilder& other ) {
        m_data = other.m_data;
        m_exprComponents = other.m_exprComponents;
        m_stream.clear();
        m_stream << other.m_stream.str();
        return *this;
    }
    AssertionResultBuilder& AssertionResultBuilder::setResultType( ResultWas::OfType result ) {
        m_data.resultType = result;
        return *this;
    }
    AssertionResultBuilder& AssertionResultBuilder::setResultType( bool result ) {
        m_data.resultType = result ? ResultWas::Ok : ResultWas::ExpressionFailed;
        return *this;
    }
    AssertionResultBuilder& AssertionResultBuilder::negate( bool shouldNegate ) {
        m_exprComponents.shouldNegate = shouldNegate;
        return *this;
    }
    AssertionResultBuilder& AssertionResultBuilder::setLhs( const std::string& lhs ) {
        m_exprComponents.lhs = lhs;
        return *this;
    }
    AssertionResultBuilder& AssertionResultBuilder::setRhs( const std::string& rhs ) {
        m_exprComponents.rhs = rhs;
        return *this;
    }
    AssertionResultBuilder& AssertionResultBuilder::setOp( const std::string& op ) {
        m_exprComponents.op = op;
        return *this;
    }
    AssertionResultData AssertionResultBuilder::build( const AssertionInfo& info ) const
    {
        assert( m_data.resultType != ResultWas::Unknown );

        AssertionResultData data = m_data;
        
        // Flip bool results if shouldNegate is set
        if( m_exprComponents.shouldNegate && data.resultType == ResultWas::Ok )
            data.resultType = ResultWas::ExpressionFailed;
        else if( m_exprComponents.shouldNegate && data.resultType == ResultWas::ExpressionFailed )
            data.resultType = ResultWas::Ok;

        data.message = m_stream.str();
        data.reconstructedExpression = reconstructExpression( info );
        if( m_exprComponents.shouldNegate ) {
            if( m_exprComponents.op == "" )
                data.reconstructedExpression = "!" + data.reconstructedExpression;
            else
                data.reconstructedExpression = "!(" + data.reconstructedExpression + ")";
        }
        return data;
    }
    std::string AssertionResultBuilder::reconstructExpression( const AssertionInfo& info ) const {
        if( m_exprComponents.op == "" )
            return m_exprComponents.lhs.empty() ? info.capturedExpression : m_exprComponents.op + m_exprComponents.lhs;
        else if( m_exprComponents.op == "matches" )
            return m_exprComponents.lhs + " " + m_exprComponents.rhs;
        else if( m_exprComponents.op != "!" ) {
            if( m_exprComponents.lhs.size() + m_exprComponents.rhs.size() < 30 )
                return m_exprComponents.lhs + " " + m_exprComponents.op + " " + m_exprComponents.rhs;
            else if( m_exprComponents.lhs.size() < 70 && m_exprComponents.rhs.size() < 70 )
                return "\n\t" + m_exprComponents.lhs + "\n\t" + m_exprComponents.op + "\n\t" + m_exprComponents.rhs;
            else
                return "\n" + m_exprComponents.lhs + "\n" + m_exprComponents.op + "\n" + m_exprComponents.rhs + "\n\n";
        }
        else
            return "{can't expand - use " + info.macroName + "_FALSE( " + info.capturedExpression.substr(1) + " ) instead of " + info.macroName + "( " + info.capturedExpression + " ) for better diagnostics}";
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_ASSERTIONRESULT_BUILDER_HPP_INCLUDED
