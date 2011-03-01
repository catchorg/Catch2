/*
 *  catch_resultinfo.hpp
 *  Catch
 *
 *  Created by Phil on 28/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_RESULT_INFO_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_RESULT_INFO_HPP_INCLUDED

#include <string>
#include "catch_result_type.h"

namespace Catch
{    
    class ResultInfo
    {
    public:
        
        ///////////////////////////////////////////////////////////////////////////
        ResultInfo
        ()
        :   m_line( 0 ),
            m_result( ResultWas::Unknown ),
            m_isNot( false ),
            m_expressionIncomplete( false )
        {}
        
        ///////////////////////////////////////////////////////////////////////////
        ResultInfo
        (
            const char* expr, 
            ResultWas::OfType result, 
            bool isNot, 
            const char* filename, 
            std::size_t line, 
            const char* macroName 
        )
        :   m_macroName( macroName ),
            m_filename( filename ),
            m_line( line ),
            m_expr( expr ),
            m_op( isNotExpression( expr ) ? "!" : "" ),
            m_result( result ),
            m_isNot( isNot ),
            m_expressionIncomplete( false )
        {
            if( isNot )
                m_expr = "!" + m_expr;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        bool ok
        ()
        const
        {
            return ( m_result & ResultWas::FailureBit ) != ResultWas::FailureBit;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        ResultWas::OfType getResultType
        ()
        const
        {
            return m_result;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        bool hasExpression
        ()
        const
        {
            return !m_expr.empty();
        }

        ///////////////////////////////////////////////////////////////////////////
        bool hasMessage
        ()
        const
        {
            return !m_message.empty();
        }

        ///////////////////////////////////////////////////////////////////////////
        std::string getExpression
        ()
        const
        {
            return m_expr;
        }

        ///////////////////////////////////////////////////////////////////////////
        std::string getExpandedExpression
        ()
        const
        {
            if( !hasExpression() )
                return "";
            
            return m_expressionIncomplete
                ? getExpandedExpressionInternal() + " {can't expand the rest of the expression - consider rewriting it}"
                : getExpandedExpressionInternal();
        }
        
        ///////////////////////////////////////////////////////////////////////////
        std::string getMessage
        ()
        const
        {
            return m_message;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        std::string getFilename
        ()
        const
        {
            return m_filename;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        std::size_t getLine
        ()
        const
        {
            return m_line;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        std::string getTestMacroName
        ()
        const
        {
            return m_macroName;
        }

    protected:

        ///////////////////////////////////////////////////////////////////////////
        std::string getExpandedExpressionInternal
        ()
        const
        {
            if( m_op == "" || m_isNot )
                return m_lhs.empty() ? m_expr : m_op + m_lhs;
            else if( m_op != "!" )
                return m_lhs + " " + m_op + " " + m_rhs;
            else
                return "{can't expand - use " + m_macroName + "_NOT( " + m_expr.substr(1) + " ) instead of " + m_macroName + "( " + m_expr + " ) for better diagnostics}";
        }

        ///////////////////////////////////////////////////////////////////////////
        bool isNotExpression
        (
            const char* expr
        )
        {
            return expr && expr[0] == '!';
        }        
        
    protected:
        std::string m_macroName;
        std::string m_filename;
        std::size_t m_line;
        std::string m_expr, m_lhs, m_rhs, m_op;
        std::string m_message;
        ResultWas::OfType m_result;
        bool m_isNot;
        bool m_expressionIncomplete;
    };
    
} // end namespace Catch


#endif // TWOBLUECUBES_CATCH_RESULT_INFO_HPP_INCLUDED
