/*
 *  Created by Phil on 8/8/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RESULTINFO_BUILDER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_RESULTINFO_BUILDER_HPP_INCLUDED

#include "catch_resultinfo_builder.h"

namespace Catch {

    ResultInfoBuilder::ResultInfoBuilder() {}

    ResultInfoBuilder& ResultInfoBuilder::setResultType( ResultWas::OfType result ) {
        // Flip bool results if isNot is set
        if( m_isNot && result == ResultWas::Ok )
            m_result = ResultWas::ExpressionFailed;
        else if( m_isNot && result == ResultWas::ExpressionFailed )
            m_result = ResultWas::Ok;
        else
            m_result = result;
        m_data.resultType = m_result;
        return *this;
    }
    ResultInfoBuilder& ResultInfoBuilder::setCapturedExpression( const std::string& capturedExpression ) {
        m_expr = capturedExpression;
        m_data.capturedExpression = capturedExpression;
        return *this;
    }
    ResultInfoBuilder& ResultInfoBuilder::setIsFalse( bool isFalse ) {
        m_isNot = isFalse;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::setMessage( const std::string& message ) {
        m_message = message;
        m_data.message = message;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::setLineInfo( const SourceLineInfo& lineInfo ) {
        m_lineInfo = lineInfo;
        m_data.lineInfo = lineInfo;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::setMacroName( const std::string& macroName ) {
        m_macroName = macroName;
        m_data.macroName = macroName;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::setLhs( const std::string& lhs ) {
        m_lhs = lhs;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::setRhs( const std::string& rhs ) {
        m_rhs = rhs;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::setOp( const std::string& op ) {
        m_op = op;
        return *this;
    }

    ResultInfoBuilder& captureBoolExpression( ResultInfoBuilder& builder, bool result ) {
        return builder
            .setLhs( Catch::toString( result ) )
            .setOp( builder.getIsFalse() ? "!" : "" )
            .setResultType( result ? ResultWas::Ok : ResultWas::ExpressionFailed );
    }

    const ResultInfo& ResultInfoBuilder::build() const
    {
        ResultData data = m_data;
        data.reconstructedExpression = reconstructExpression();
        return *this;
    }

    std::string ResultInfoBuilder::reconstructExpression() const {
        if( m_op == "" || m_isNot )
            return m_lhs.empty() ? m_expr : m_op + m_lhs;
        else if( m_op == "matches" )
            return m_lhs + " " + m_rhs;
        else if( m_op != "!" )
        {
            if( m_lhs.size() + m_rhs.size() < 30 )
                return m_lhs + " " + m_op + " " + m_rhs;
            else if( m_lhs.size() < 70 && m_rhs.size() < 70 )
                return "\n\t" + m_lhs + "\n\t" + m_op + "\n\t" + m_rhs;
            else
                return "\n" + m_lhs + "\n" + m_op + "\n" + m_rhs + "\n\n";
        }
        else
            return "{can't expand - use " + m_data.macroName + "_FALSE( " + m_expr.substr(1) + " ) instead of " + m_data.macroName + "( " + m_expr + " ) for better diagnostics}";
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RESULTINFO_BUILDER_HPP_INCLUDED
