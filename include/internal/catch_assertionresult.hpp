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


    INTERNAL_CATCH_INLINE AssertionInfo::AssertionInfo(   std::string const& _macroName,
                                    SourceLineInfo const& _lineInfo,
                                    std::string const& _capturedExpression,
                                    ResultDisposition::Flags _resultDisposition )
    :   macroName( _macroName ),
        lineInfo( _lineInfo ),
        capturedExpression( _capturedExpression ),
        resultDisposition( _resultDisposition )
    {}

    INTERNAL_CATCH_INLINE AssertionResult::AssertionResult() {}

    INTERNAL_CATCH_INLINE AssertionResult::AssertionResult( AssertionInfo const& info, AssertionResultData const& data )
    :   m_info( info ),
        m_resultData( data )
    {}

    INTERNAL_CATCH_INLINE AssertionResult::~AssertionResult() {}

    // Result was a success
    INTERNAL_CATCH_INLINE bool AssertionResult::succeeded() const {
        return Catch::isOk( m_resultData.resultType );
    }

    // Result was a success, or failure is suppressed
    INTERNAL_CATCH_INLINE bool AssertionResult::isOk() const {
        return Catch::isOk( m_resultData.resultType ) || shouldSuppressFailure( m_info.resultDisposition );
    }

    INTERNAL_CATCH_INLINE ResultWas::OfType AssertionResult::getResultType() const {
        return m_resultData.resultType;
    }

    INTERNAL_CATCH_INLINE bool AssertionResult::hasExpression() const {
        return !m_info.capturedExpression.empty();
    }

    INTERNAL_CATCH_INLINE bool AssertionResult::hasMessage() const {
        return !m_resultData.message.empty();
    }

    INTERNAL_CATCH_INLINE std::string AssertionResult::getExpression() const {
        if( shouldNegate( m_info.resultDisposition ) )
            return "!" + m_info.capturedExpression;
        else
            return m_info.capturedExpression;
    }
    INTERNAL_CATCH_INLINE std::string AssertionResult::getExpressionInMacro() const {
        if( m_info.macroName.empty() )
            return m_info.capturedExpression;
        else
            return m_info.macroName + "( " + m_info.capturedExpression + " )";
    }

    INTERNAL_CATCH_INLINE bool AssertionResult::hasExpandedExpression() const {
        return hasExpression() && getExpandedExpression() != getExpression();
    }

    INTERNAL_CATCH_INLINE std::string AssertionResult::getExpandedExpression() const {
        return m_resultData.reconstructedExpression;
    }

    INTERNAL_CATCH_INLINE std::string AssertionResult::getMessage() const {
        return m_resultData.message;
    }
    INTERNAL_CATCH_INLINE SourceLineInfo AssertionResult::getSourceInfo() const {
        return m_info.lineInfo;
    }

    INTERNAL_CATCH_INLINE std::string AssertionResult::getTestMacroName() const {
        return m_info.macroName;
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_ASSERTIONRESULT_HPP_INCLUDED
