/*
 *  Created by Phil on 28/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RESULT_INFO_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_RESULT_INFO_HPP_INCLUDED

#include <string>
#include "catch_result_type.h"

namespace Catch {

    class ResultInfo {
    public:
        ResultInfo()
        :   m_macroName(),
            m_expr(),
            m_lhs(),
            m_rhs(),
            m_op(),
            m_message(),
            m_result( ResultWas::Unknown ),
            m_isNot( false )
        {}
        
        ResultInfo( const char* expr, 
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
        
        virtual ~ResultInfo() {}
        
        bool ok() const {
            return ( m_result & ResultWas::FailureBit ) != ResultWas::FailureBit;
        }
        
        ResultWas::OfType getResultType() const {
            return m_result;
        }
        
        bool hasExpression() const {
            return !m_expr.empty();
        }

        bool hasMessage() const {
            return !m_message.empty();
        }

        std::string getExpression() const {
            return m_expr;
        }

        bool hasExpandedExpression() const {
            return hasExpression() && getExpandedExpressionInternal() != m_expr;
        }
        
        std::string getExpandedExpression() const {
            return hasExpression() ? getExpandedExpressionInternal() : std::string();
        }
        
        std::string getMessage() const {
            return m_message;
        }
        
        std::string getFilename() const {
            return m_lineInfo.file;
        }
        
        std::size_t getLine() const {
            return m_lineInfo.line;
        }
        
        std::string getTestMacroName() const {
            return m_macroName;
        }

    protected:

        std::string getExpandedExpressionInternal() const {
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

        bool isNotExpression( const char* expr ) {
            return expr && expr[0] == '!';
        }        
        
    protected:
        std::string m_macroName;
        SourceLineInfo m_lineInfo;
        std::string m_expr, m_lhs, m_rhs, m_op;
        std::string m_message;
        ResultWas::OfType m_result;
        bool m_isNot;
    };
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RESULT_INFO_HPP_INCLUDED
