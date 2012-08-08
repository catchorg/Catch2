/*
 *  Created by Phil on 8/8/12
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RESULT_INFO_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_RESULT_INFO_HPP_INCLUDED

#include "catch_resultinfo.h"

namespace Catch {

    ResultInfo::ResultInfo()
    :   m_macroName(),
        m_expr(),
        m_lhs(),
        m_rhs(),
        m_op(),
        m_message(),
        m_result( ResultWas::Unknown ),
        m_isNot( false )
        {}

    ResultInfo::ResultInfo(const char* expr,
                           ResultWas::OfType result,
                           bool isNot,
                           const SourceLineInfo& lineInfo,
                           const char* macroName,
                           const char* message )
    :   m_macroName( macroName ),
        m_lineInfo( lineInfo ),
        m_expr( expr ),
        m_lhs(),
        m_rhs(),
        m_op( isNotExpression( expr ) ? "!" : "" ),
        m_message( message ),
        m_result( result ),
        m_isNot( isNot )
        {
            if( isNot )
                m_expr = "!" + m_expr;
        }

    ResultInfo::~ResultInfo() {}

    bool ResultInfo::ok() const {
        return ( m_result & ResultWas::FailureBit ) != ResultWas::FailureBit;
    }

    ResultWas::OfType ResultInfo::getResultType() const {
        return m_result;
    }

    bool ResultInfo::hasExpression() const {
        return !m_expr.empty();
    }

    bool ResultInfo::hasMessage() const {
        return !m_message.empty();
    }

    std::string ResultInfo::getExpression() const {
        return m_expr;
    }

    bool ResultInfo::hasExpandedExpression() const {
        return hasExpression() && getExpandedExpressionInternal() != m_expr;
    }

    std::string ResultInfo::getExpandedExpression() const {
        return hasExpression() ? getExpandedExpressionInternal() : "";
    }

    std::string ResultInfo::getMessage() const {
        return m_message;
    }

    std::string ResultInfo::getFilename() const {
        return m_lineInfo.file;
    }

    std::size_t ResultInfo::getLine() const {
        return m_lineInfo.line;
    }

    std::string ResultInfo::getTestMacroName() const {
        return m_macroName;
    }

    std::string ResultInfo::getExpandedExpressionInternal() const {
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
            return "{can't expand - use " + m_macroName + "_FALSE( " + m_expr.substr(1) + " ) instead of " + m_macroName + "( " + m_expr + " ) for better diagnostics}";
    }

    bool ResultInfo::isNotExpression( const char* expr ) {
        return expr && expr[0] == '!';
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RESULT_INFO_HPP_INCLUDED
