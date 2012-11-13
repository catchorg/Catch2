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


    AssertionInfo::AssertionInfo(   const std::string& _macroName,
                                    const SourceLineInfo& _lineInfo,
                                    const std::string& _capturedExpression,
                                    ResultDisposition::Flags _resultDisposition )
    :   macroName( _macroName ),
        lineInfo( _lineInfo ),
        capturedExpression( _capturedExpression ),
        resultDisposition( _resultDisposition )
    {
        if( shouldNegate( resultDisposition ) )
            capturedExpression = "!" + _capturedExpression;
    }

    AssertionResult::AssertionResult() {}

    AssertionResult::AssertionResult( const AssertionInfo& info, const AssertionResultData& data )
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
        return !m_info.capturedExpression.empty();
    }

    bool AssertionResult::hasMessage() const {
        return !m_resultData.message.empty();
    }

    std::string AssertionResult::getExpression() const {
        return m_info.capturedExpression;
    }

    bool AssertionResult::hasExpandedExpression() const {
        return hasExpression() && getExpandedExpression() != getExpression();
    }

    std::string AssertionResult::getExpandedExpression() const {
        return m_resultData.reconstructedExpression;
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

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_ASSERTIONRESULT_HPP_INCLUDED
