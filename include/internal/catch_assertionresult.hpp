/*
 *  Created by Phil on 8/8/12
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_ASSERTIONRESULT_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_ASSERTIONRESULT_HPP_INCLUDED

#include "catch_assertionresult.h"

namespace Catch {


    AssertionInfo::AssertionInfo():macroName(""), capturedExpression(""), resultDisposition(ResultDisposition::Normal), secondArg(""){}

    AssertionInfo::AssertionInfo(   char const * _macroName,
                                    SourceLineInfo const& _lineInfo,
                                    char const * _capturedExpression,
                                    ResultDisposition::Flags _resultDisposition,
                                    char const * _secondArg)
    :   macroName( _macroName ),
        lineInfo( _lineInfo ),
        capturedExpression( _capturedExpression ),
        resultDisposition( _resultDisposition ),
        secondArg( _secondArg )
    {}

    AssertionResult::AssertionResult() {}

    AssertionResult::AssertionResult( AssertionInfo const& info, AssertionResultData const& data )
    :   m_info( info ),
        m_resultData( data )
    {}

    AssertionResult::~AssertionResult() {}

    // Result was a success
    bool AssertionResult::succeeded() const {
        return Catch::isOk( m_resultData.resultType );
    }

    // Result was a success, or failure is suppressed
    bool AssertionResult::isOk() const {
        return Catch::isOk( m_resultData.resultType ) || shouldSuppressFailure( m_info.resultDisposition );
    }

    ResultWas::OfType AssertionResult::getResultType() const {
        return m_resultData.resultType;
    }

    bool AssertionResult::hasExpression() const {
        return m_info.capturedExpression[0] != 0;
    }

    bool AssertionResult::hasMessage() const {
        return !m_resultData.message.empty();
    }

    std::string capturedExpressionWithSecondArgument( char const * capturedExpression, char const * secondArg ) {
        return (secondArg[0] == 0 || secondArg[0] == '"' && secondArg[1] == '"')
            ? capturedExpression
            : std::string(capturedExpression) + ", " + secondArg;
    }

    std::string AssertionResult::getExpression() const {
        if( isFalseTest( m_info.resultDisposition ) )
            return "!(" + capturedExpressionWithSecondArgument(m_info.capturedExpression, m_info.secondArg) + ")";
        else
            return capturedExpressionWithSecondArgument(m_info.capturedExpression, m_info.secondArg);
    }
    std::string AssertionResult::getExpressionInMacro() const {
        if( m_info.macroName[0] == 0 )
            return capturedExpressionWithSecondArgument(m_info.capturedExpression, m_info.secondArg);
        else
            return std::string(m_info.macroName) + "( " + capturedExpressionWithSecondArgument(m_info.capturedExpression, m_info.secondArg) + " )";
    }

    bool AssertionResult::hasExpandedExpression() const {
        return hasExpression() && getExpandedExpression() != getExpression();
    }

    std::string AssertionResult::getExpandedExpression() const {
        return m_resultData.reconstructExpression();
    }

    std::string AssertionResult::getMessage() const {
        return m_resultData.message;
    }
    SourceLineInfo AssertionResult::getSourceInfo() const {
        return m_info.lineInfo;
    }

    std::string AssertionResult::getTestMacroName() const {
        return m_info.macroName;
    }

    void AssertionResult::discardDecomposedExpression() const {
        m_resultData.decomposedExpression = CATCH_NULL;
    }

    void AssertionResult::expandDecomposedExpression() const {
        m_resultData.reconstructExpression();
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_ASSERTIONRESULT_HPP_INCLUDED
