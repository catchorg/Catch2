/*
 *  Created by Phil on 8/8/12
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_assertionresult.h"

namespace Catch {


    bool DecomposedExpression::isBinaryExpression() const {
        return false;
    }

    AssertionInfo::AssertionInfo(   char const * _macroName,
                                    SourceLineInfo const& _lineInfo,
                                    char const * _capturedExpression,
                                    ResultDisposition::Flags _resultDisposition)
    :   macroName( _macroName ),
        lineInfo( _lineInfo ),
        capturedExpression( _capturedExpression ),
        resultDisposition( _resultDisposition )
    {}

    void AssertionResultData::negate( bool parenthesize ) {
        negated = !negated;
        parenthesized = parenthesize;
        if( resultType == ResultWas::Ok )
            resultType = ResultWas::ExpressionFailed;
        else if( resultType == ResultWas::ExpressionFailed )
            resultType = ResultWas::Ok;
    }

    std::string const& AssertionResultData::reconstructExpression() const {
        if( decomposedExpression != nullptr ) {
            decomposedExpression->reconstructExpression( reconstructedExpression );
            if( parenthesized ) {
                reconstructedExpression.insert( 0, 1, '(' );
                reconstructedExpression.append( 1, ')' );
            }
            if( negated ) {
                reconstructedExpression.insert( 0, 1, '!' );
            }
            decomposedExpression = nullptr;
        }
        return reconstructedExpression;
    }

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

    std::string AssertionResult::getExpression() const {
        if (isFalseTest(m_info.resultDisposition))
            return '!' + std::string(m_info.capturedExpression);
        else
            return std::string(m_info.capturedExpression);
    }

    std::string AssertionResult::getExpressionInMacro() const {
        if( m_info.macroName[0] == 0 )
            return std::string(m_info.capturedExpression);
        else
            return std::string(m_info.macroName) + "( " + m_info.capturedExpression + " )";
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
        m_resultData.decomposedExpression = nullptr;
    }

    void AssertionResult::expandDecomposedExpression() const {
        m_resultData.reconstructExpression();
    }

} // end namespace Catch
