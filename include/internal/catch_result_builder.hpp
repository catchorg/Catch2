/*
 *  Created by Phil on 28/5/2014.
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RESULT_BUILDER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_RESULT_BUILDER_HPP_INCLUDED

#include "catch_result_builder.h"
#include "catch_context.h"
#include "catch_interfaces_config.h"
#include "catch_interfaces_runner.h"
#include "catch_interfaces_capture.h"
#include "catch_interfaces_registry_hub.h"


namespace Catch {

    ResultBuilder::ResultBuilder(   char const* macroName,
                                    SourceLineInfo const& lineInfo,
                                    char const* capturedExpression,
                                    ResultDisposition::Flags resultDisposition )
    :   m_assertionInfo( macroName, lineInfo, capturedExpression, resultDisposition ),
        m_shouldDebugBreak( false ),
        m_shouldThrow( false )
    {}

    ResultBuilder& ResultBuilder::setResultType( ResultWas::OfType result ) {
        m_data.resultType = result;
        return *this;
    }
    ResultBuilder& ResultBuilder::setResultType( bool result ) {
        m_data.resultType = result ? ResultWas::Ok : ResultWas::ExpressionFailed;
        return *this;
    }
    ResultBuilder& ResultBuilder::setLhs( std::string const& lhs ) {
        m_exprComponents.lhs = lhs;
        return *this;
    }
    ResultBuilder& ResultBuilder::setRhs( std::string const& rhs ) {
        m_exprComponents.rhs = rhs;
        return *this;
    }
    ResultBuilder& ResultBuilder::setOp( std::string const& op ) {
        m_exprComponents.op = op;
        return *this;
    }

    void ResultBuilder::endExpression() {
        m_exprComponents.testFalse = isFalseTest( m_assertionInfo.resultDisposition );
        captureExpression();
    }

    void ResultBuilder::useActiveException( ResultDisposition::Flags resultDisposition ) {
        m_assertionInfo.resultDisposition = resultDisposition;
        m_stream.oss << Catch::translateActiveException();
        captureResult( ResultWas::ThrewException );
    }

    void ResultBuilder::captureResult( ResultWas::OfType resultType ) {
        setResultType( resultType );
        captureExpression();
    }

    void ResultBuilder::captureExpression() {
        AssertionResult result = build();
        getResultCapture().assertionEnded( result );

        if( !result.isOk() ) {
            if( getCurrentContext().getConfig()->shouldDebugBreak() )
                m_shouldDebugBreak = true;
            if( getCurrentContext().getRunner()->aborting() || (m_assertionInfo.resultDisposition & ResultDisposition::Normal) )
                m_shouldThrow = true;
        }
    }
    void ResultBuilder::react() {
        if( m_shouldThrow )
            throw Catch::TestFailureException();
    }

    bool ResultBuilder::shouldDebugBreak() const { return m_shouldDebugBreak; }
    bool ResultBuilder::allowThrows() const { return getCurrentContext().getConfig()->allowThrows(); }

    AssertionResult ResultBuilder::build() const
    {
        assert( m_data.resultType != ResultWas::Unknown );

        AssertionResultData data = m_data;

        // Flip bool results if testFalse is set
        if( m_exprComponents.testFalse ) {
            if( data.resultType == ResultWas::Ok )
                data.resultType = ResultWas::ExpressionFailed;
            else if( data.resultType == ResultWas::ExpressionFailed )
                data.resultType = ResultWas::Ok;
        }

        data.message = m_stream.oss.str();
        data.reconstructedExpression = reconstructExpression();
        if( m_exprComponents.testFalse ) {
            if( m_exprComponents.op == "" )
                data.reconstructedExpression = "!" + data.reconstructedExpression;
            else
                data.reconstructedExpression = "!(" + data.reconstructedExpression + ")";
        }
        return AssertionResult( m_assertionInfo, data );
    }
    std::string ResultBuilder::reconstructExpression() const {
        if( m_exprComponents.op == "" )
            return m_exprComponents.lhs.empty() ? m_assertionInfo.capturedExpression : m_exprComponents.op + m_exprComponents.lhs;
        else if( m_exprComponents.op == "matches" )
            return m_exprComponents.lhs + " " + m_exprComponents.rhs;
        else if( m_exprComponents.op != "!" ) {
            if( m_exprComponents.lhs.size() + m_exprComponents.rhs.size() < 40 &&
                m_exprComponents.lhs.find("\n") == std::string::npos &&
                m_exprComponents.rhs.find("\n") == std::string::npos )
                return m_exprComponents.lhs + " " + m_exprComponents.op + " " + m_exprComponents.rhs;
            else
                return m_exprComponents.lhs + "\n" + m_exprComponents.op + "\n" + m_exprComponents.rhs;
        }
        else
            return "{can't expand - use " + m_assertionInfo.macroName + "_FALSE( " + m_assertionInfo.capturedExpression.substr(1) + " ) instead of " + m_assertionInfo.macroName + "( " + m_assertionInfo.capturedExpression + " ) for better diagnostics}";
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RESULT_BUILDER_HPP_INCLUDED
