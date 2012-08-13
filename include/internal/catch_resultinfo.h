/*
 *  Created by Phil on 28/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RESULT_INFO_H_INCLUDED
#define TWOBLUECUBES_CATCH_RESULT_INFO_H_INCLUDED

#include <string>
#include "catch_result_type.h"

namespace Catch {

    class ResultInfo {
    public:
        ResultInfo();        
        ResultInfo( const char* expr, 
                    ResultWas::OfType result, 
                    bool isNot,
                    const SourceLineInfo& lineInfo,
                    const char* macroName,
                   const char* message );
        ~ResultInfo();
        
        bool ok() const;
        ResultWas::OfType getResultType() const;
        bool hasExpression() const;
        bool hasMessage() const;
        std::string getExpression() const;
        bool hasExpandedExpression() const;
        std::string getExpandedExpression() const;
        std::string getMessage() const;
        std::string getFilename() const;
        std::size_t getLine() const;
        std::string getTestMacroName() const;

    protected:

        std::string getExpandedExpressionInternal() const;
        bool isNotExpression( const char* expr );
        
    protected:
        std::string m_macroName;
        SourceLineInfo m_lineInfo;
        std::string m_expr, m_lhs, m_rhs, m_op;
        std::string m_message;
        ResultWas::OfType m_result;
        bool m_isNot;
    };
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RESULT_INFO_H_INCLUDED
