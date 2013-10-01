/*
 *  Created by Phil on 8/8/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_EXPRESSIONRESULT_BUILDER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_EXPRESSIONRESULT_BUILDER_HPP_INCLUDED

#include "catch_expressionresult_builder.h"

#include <assert.h>

namespace Catch {

    ExpressionResultBuilder::ExpressionResultBuilder( ResultWas::OfType resultType ) {
        m_data.resultType = resultType;
    }
    ExpressionResultBuilder::ExpressionResultBuilder( ExpressionResultBuilder const& other )
    :   m_data( other.m_data ),
        m_exprComponents( other.m_exprComponents )
    {
        m_stream << other.m_stream.str();
    }
    ExpressionResultBuilder& ExpressionResultBuilder::operator=(ExpressionResultBuilder const& other ) {
        m_data = other.m_data;
        m_exprComponents = other.m_exprComponents;
        m_stream.str("");
        m_stream << other.m_stream.str();
        return *this;
    }
    ExpressionResultBuilder& ExpressionResultBuilder::setResultType( ResultWas::OfType result ) {
        m_data.resultType = result;
        return *this;
    }
    ExpressionResultBuilder& ExpressionResultBuilder::setResultType( bool result ) {
        m_data.resultType = result ? ResultWas::Ok : ResultWas::ExpressionFailed;
        return *this;
    }
    ExpressionResultBuilder& ExpressionResultBuilder::endExpression( ResultDisposition::Flags resultDisposition ) {
        m_exprComponents.shouldNegate = shouldNegate( resultDisposition );
        return *this;
    }
    ExpressionResultBuilder& ExpressionResultBuilder::setLhs( std::string const& lhs ) {
        m_exprComponents.lhs = lhs;
        return *this;
    }
    ExpressionResultBuilder& ExpressionResultBuilder::setRhs( std::string const& rhs ) {
        m_exprComponents.rhs = rhs;
        return *this;
    }
    ExpressionResultBuilder& ExpressionResultBuilder::setOp( std::string const& op ) {
        m_exprComponents.op = op;
        return *this;
    }
    AssertionResult ExpressionResultBuilder::buildResult( AssertionInfo const& info ) const
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
        return AssertionResult( info, data );
    }
    std::string ExpressionResultBuilder::reconstructExpression( AssertionInfo const& info ) const {
        if( m_exprComponents.op == "" )
            return m_exprComponents.lhs.empty() ? info.capturedExpression : m_exprComponents.op + m_exprComponents.lhs;
        else if( m_exprComponents.op == "matches" )
            return m_exprComponents.lhs + " " + m_exprComponents.rhs;
        else if( m_exprComponents.op != "!" ) {
            if( m_exprComponents.lhs.size() + m_exprComponents.rhs.size() < 40 &&
                m_exprComponents.lhs.find("\n") == std::string::npos &&
                m_exprComponents.rhs.find("\n") == std::string::npos )
                return m_exprComponents.lhs + " " + m_exprComponents.op + " " + m_exprComponents.rhs;
            else
                return m_exprComponents.lhs + "\n" + m_exprComponents.op + "\n" + m_exprComponents.rhs;
        }
        else
            return "{can't expand - use " + info.macroName + "_FALSE( " + info.capturedExpression.substr(1) + " ) instead of " + info.macroName + "( " + info.capturedExpression + " ) for better diagnostics}";
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_EXPRESSIONRESULT_BUILDER_HPP_INCLUDED
