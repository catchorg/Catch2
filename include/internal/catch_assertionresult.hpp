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

    AssertionResult::AssertionResult() {}

    AssertionResult::AssertionResult( const ResultData& data ) : m_data( data ) {}

    AssertionResult::~AssertionResult() {}

    bool AssertionResult::ok() const {
        return isOk( m_data.resultType );
    }

    ResultWas::OfType AssertionResult::getResultType() const {
        return m_data.resultType;
    }

    bool AssertionResult::hasExpression() const {
        return !m_data.capturedExpression.empty();
    }

    bool AssertionResult::hasMessage() const {
        return !m_data.message.empty();
    }

    std::string AssertionResult::getExpression() const {
        return m_data.capturedExpression;
    }

    bool AssertionResult::hasExpandedExpression() const {
        return hasExpression() && getExpandedExpression() != getExpression();
    }

    std::string AssertionResult::getExpandedExpression() const {
        return m_data.reconstructedExpression;
    }

    std::string AssertionResult::getMessage() const {
        return m_data.message;
    }

    std::string AssertionResult::getFilename() const {
        return m_data.lineInfo.file;
    }

    std::size_t AssertionResult::getLine() const {
        return m_data.lineInfo.line;
    }

    std::string AssertionResult::getTestMacroName() const {
        return m_data.macroName;
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_ASSERTIONRESULT_HPP_INCLUDED
