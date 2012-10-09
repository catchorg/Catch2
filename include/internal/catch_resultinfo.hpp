/*
 *  Created by Phil on 8/8/12
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RESULTINFO_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_RESULTINFO_HPP_INCLUDED

#include "catch_resultinfo.h"

namespace Catch {

    ResultInfo::ResultInfo() {}

    ResultInfo::ResultInfo( const ResultData& data ) : m_data( data ) {}

    ResultInfo::~ResultInfo() {}

    bool ResultInfo::ok() const {
        return isOk( m_data.resultType );
    }

    ResultWas::OfType ResultInfo::getResultType() const {
        return m_data.resultType;
    }

    bool ResultInfo::hasExpression() const {
        return !m_data.capturedExpression.empty();
    }

    bool ResultInfo::hasMessage() const {
        return !m_data.message.empty();
    }

    std::string ResultInfo::getExpression() const {
        return m_data.capturedExpression;
    }

    bool ResultInfo::hasExpandedExpression() const {
        return hasExpression() && getExpandedExpression() != getExpression();
    }

    std::string ResultInfo::getExpandedExpression() const {
        return m_data.reconstructedExpression;
    }

    std::string ResultInfo::getMessage() const {
        return m_data.message;
    }

    std::string ResultInfo::getFilename() const {
        return m_data.lineInfo.file;
    }

    std::size_t ResultInfo::getLine() const {
        return m_data.lineInfo.line;
    }

    std::string ResultInfo::getTestMacroName() const {
        return m_data.macroName;
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RESULTINFO_HPP_INCLUDED
