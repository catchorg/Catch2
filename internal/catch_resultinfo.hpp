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

namespace Catch
{    
    struct ResultWas{ enum OfType
        {
            Unknown = -1,
            Ok = 0,
            ExpressionFailed = 1,
            
            Info = 2,
            Warning = 3,
            ExplicitFailure = 4,
            
            Exception = 0x100,
            
            ThrewException = Exception | 1,
            DidntThrowException = Exception | 2
            
        }; };
    
    class ResultInfo
    {
    public:
        
        ResultInfo()
        :   m_result( ResultWas::Unknown ),
            m_isNot( false ),
            m_line( 0 ),
            m_expressionIncomplete( false )
        {}
        
        ResultInfo( const std::string& expr, ResultWas::OfType result, bool isNot, const std::string& filename, std::size_t line, const std::string& macroName )
        :   m_expr( expr ),
            m_result( result ),
            m_isNot( isNot ),
            m_op( m_expr[0] == '!' ? "!" : "" ),
            m_filename( filename ),
            m_line( line ), 
            m_macroName( macroName ),
            m_expressionIncomplete( false )
        {
        }
        
        bool ok() const
        {
            return m_result == ResultWas::Ok;
        }
        
        ResultWas::OfType getResultType() const
        {
            return m_result;
        }
        
        bool hasExpression() const
        {
            return !m_expr.empty();
        }
        bool hasMessage() const
        {
            return !m_message.empty();
        }
        std::string getExpression() const
        {
            return m_expr;
        }
        std::string getExpandedExpression() const
        {
            return m_expressionIncomplete
                ? getExpandedExpressionInternal() + " {can't expand the rest of the expression - consider rewriting it}"
                : getExpandedExpressionInternal();
        }
        
        std::string getMessage() const
        {
            return m_message;
        }
        
        std::string getFilename() const
        {
            return m_filename;
        }
        
        std::size_t getLine() const
        {
            return m_line;
        }
        
        std::string getTestMacroName() const
        {
            return m_macroName;
        }

    protected:
        std::string getExpandedExpressionInternal() const
        {
            if( m_op == "" || m_isNot )
                return m_lhs.empty() ? m_expr : m_op + m_lhs;
            else if( m_op != "!" )
                return m_lhs + " " + m_op + " " + m_rhs;
            else
                return "{can't expand - use " + m_macroName + "_NOT( " + m_expr.substr(1) + " ) instead of " + m_macroName + "( " + m_expr + " ) for better diagnostics}";
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
