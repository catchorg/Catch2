/*
 *  Created by Phil on 8/8/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RESULTINFO_BUILDER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_RESULTINFO_BUILDER_HPP_INCLUDED

#include "catch_resultinfo_builder.h"

namespace Catch {

    ResultInfoBuilder::ResultInfoBuilder(   const char* expr,
                                            bool isNot,
                                            const SourceLineInfo& lineInfo,
                                            const char* macroName )
    : ResultInfo( expr, ResultWas::Unknown, isNot, lineInfo, macroName, "" )
    {}

    ResultInfoBuilder::ResultInfoBuilder() {}

    ResultInfoBuilder& ResultInfoBuilder::setResultType( ResultWas::OfType result ) {
        // Flip bool results if isNot is set
        if( m_isNot && result == ResultWas::Ok )
            m_result = ResultWas::ExpressionFailed;
        else if( m_isNot && result == ResultWas::ExpressionFailed )
            m_result = ResultWas::Ok;
        else
            m_result = result;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::setMessage( const std::string& message ) {
        m_message = message;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::setLineInfo( const SourceLineInfo& lineInfo ) {
        m_lineInfo = lineInfo;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::setLhs( const std::string& lhs ) {
        m_lhs = lhs;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::setRhs( const std::string& rhs ) {
        m_rhs = rhs;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::setOp( const std::string& op ) {
        m_op = op;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::setMacroName( const std::string& macroName ) {
        m_macroName = macroName;
        return *this;
    }

    ResultInfoBuilder& ResultInfoBuilder::captureBoolExpression( bool result ) {
        m_lhs = Catch::toString( result );
        m_op = m_isNot ? "!" : "";
        setResultType( result ? ResultWas::Ok : ResultWas::ExpressionFailed );
        return *this;
    }
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RESULTINFO_BUILDER_HPP_INCLUDED
