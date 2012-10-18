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

namespace Catch {

    AssertionResultBuilder::AssertionResultBuilder() {}
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
        // Flip bool results if isFalse is set
        if( m_exprComponents.isFalse && result == ResultWas::Ok )
            m_data.resultType = ResultWas::ExpressionFailed;
        else if( m_exprComponents.isFalse && result == ResultWas::ExpressionFailed )
            m_data.resultType = ResultWas::Ok;
        else
            m_data.resultType = result;
        return *this;
    }
    AssertionResultBuilder& AssertionResultBuilder::setCapturedExpression( const std::string& capturedExpression ) {
        m_data.capturedExpression = capturedExpression;
        return *this;
    }
    AssertionResultBuilder& AssertionResultBuilder::setIsFalse( bool isFalse ) {
        m_exprComponents.isFalse = isFalse;
        return *this;
    }

    AssertionResultBuilder& AssertionResultBuilder::setLineInfo( const SourceLineInfo& lineInfo ) {
        m_data.lineInfo = lineInfo;
        return *this;
    }

    AssertionResultBuilder& AssertionResultBuilder::setMacroName( const std::string& macroName ) {
        m_data.macroName = macroName;
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

    AssertionResult AssertionResultBuilder::build() const
    {
        AssertionResultData data = m_data;
        data.message = m_stream.str();
        data.reconstructedExpression = reconstructExpression();
        if( m_exprComponents.isFalse ) {
            if( m_exprComponents.op == "" ) {
                data.capturedExpression = "!" + data.capturedExpression;
                data.reconstructedExpression = "!" + data.reconstructedExpression;
            }
            else {
                data.capturedExpression = "!(" + data.capturedExpression + ")";
                data.reconstructedExpression = "!(" + data.reconstructedExpression + ")";
            }
        }
        return AssertionResult( data );
    }

    std::string AssertionResultBuilder::reconstructExpression() const {
        if( m_exprComponents.op == "" )
            return m_exprComponents.lhs.empty() ? m_data.capturedExpression : m_exprComponents.op + m_exprComponents.lhs;
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
            return "{can't expand - use " + m_data.macroName + "_FALSE( " + m_data.capturedExpression.substr(1) + " ) instead of " + m_data.macroName + "( " + m_data.capturedExpression + " ) for better diagnostics}";
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_ASSERTIONRESULT_BUILDER_HPP_INCLUDED
