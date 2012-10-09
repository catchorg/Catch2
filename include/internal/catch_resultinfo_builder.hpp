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
        // Flip bool results if isFalse is set
        if( m_isFalse && result == ResultWas::Ok )
            m_data.resultType = ResultWas::ExpressionFailed;
        else if( m_isFalse && result == ResultWas::ExpressionFailed )
            m_data.resultType = ResultWas::Ok;
        else
            m_data.resultType = result;
        return *this;
    }
    ResultInfoBuilder& ResultInfoBuilder::setCapturedExpression( const std::string& capturedExpression ) {
        m_data.capturedExpression = capturedExpression;
        return *this;
    }
    ResultInfoBuilder& ResultInfoBuilder::setIsFalse( bool isFalse ) {
        m_isFalse = isFalse;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::setMessage( const std::string& message ) {
        m_data.message = message;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::setLineInfo( const SourceLineInfo& lineInfo ) {
        m_data.lineInfo = lineInfo;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::setMacroName( const std::string& macroName ) {
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

    ResultInfo ResultInfoBuilder::build() const
    {
        ResultData data = m_data;
        data.reconstructedExpression = reconstructExpression();
        if( m_isFalse ) {
            if( m_op == "" ) {
                data.capturedExpression = "!" + data.capturedExpression;
                data.reconstructedExpression = "!" + data.reconstructedExpression;
            }
            else {
                data.capturedExpression = "!(" + data.capturedExpression + ")";
                data.reconstructedExpression = "!(" + data.reconstructedExpression + ")";
            }
        }
        return ResultInfo( data );
    }

    std::string ResultInfoBuilder::reconstructExpression() const {
        if( m_op == "" )
            return m_lhs.empty() ? m_data.capturedExpression : m_op + m_lhs;
        else if( m_op == "matches" )
            return m_lhs + " " + m_rhs;
        else if( m_op != "!" ) {
            if( m_lhs.size() + m_rhs.size() < 30 )
                return m_lhs + " " + m_op + " " + m_rhs;
            else if( m_lhs.size() < 70 && m_rhs.size() < 70 )
                return "\n\t" + m_lhs + "\n\t" + m_op + "\n\t" + m_rhs;
            else
                return "\n" + m_lhs + "\n" + m_op + "\n" + m_rhs + "\n\n";
        }
        else
            return "{can't expand - use " + m_data.macroName + "_FALSE( " + m_data.capturedExpression.substr(1) + " ) instead of " + m_data.macroName + "( " + m_data.capturedExpression + " ) for better diagnostics}";
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RESULTINFO_BUILDER_HPP_INCLUDED
