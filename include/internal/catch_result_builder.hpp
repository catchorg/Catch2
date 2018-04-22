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
#include "catch_wildcard_pattern.hpp"

#include <cassert>

namespace Catch {

    ResultBuilder::ResultBuilder(   char const* macroName,
                                    SourceLineInfo const& lineInfo,
                                    char const* capturedExpression,
                                    ResultDisposition::Flags resultDisposition,
                                    char const* secondArg )
    :   m_assertionInfo( macroName, lineInfo, capturedExpression, resultDisposition, secondArg ),
        m_shouldDebugBreak( false ),
        m_shouldThrow( false ),
        m_guardException( false ),
        m_usedStream( false )
    {}

    ResultBuilder::~ResultBuilder() {
#if defined(CATCH_CONFIG_FAST_COMPILE)
        if ( m_guardException ) {
            stream().oss << "Exception translation was disabled by CATCH_CONFIG_FAST_COMPILE";
            captureResult( ResultWas::ThrewException );
            getCurrentContext().getResultCapture()->exceptionEarlyReported();
        }
#endif
    }

    ResultBuilder& ResultBuilder::setResultType( ResultWas::OfType result ) {
        m_data.resultType = result;
        return *this;
    }
    ResultBuilder& ResultBuilder::setResultType( bool result ) {
        m_data.resultType = result ? ResultWas::Ok : ResultWas::ExpressionFailed;
        return *this;
    }

    void ResultBuilder::endExpression( DecomposedExpression const& expr ) {
        // Flip bool results if FalseTest flag is set
        if( isFalseTest( m_assertionInfo.resultDisposition ) ) {
            m_data.negate( expr.isBinaryExpression() );
        }

        getResultCapture().assertionRun();

        if(getCurrentContext().getConfig()->includeSuccessfulResults() || m_data.resultType != ResultWas::Ok)
        {
            AssertionResult result = build( expr );
            handleResult( result );
        }
        else
            getResultCapture().assertionPassed();
    }

    void ResultBuilder::useActiveException( ResultDisposition::Flags resultDisposition ) {
        m_assertionInfo.resultDisposition = resultDisposition;
        stream().oss << Catch::translateActiveException();
        captureResult( ResultWas::ThrewException );
    }

    void ResultBuilder::captureResult( ResultWas::OfType resultType ) {
        setResultType( resultType );
        captureExpression();
    }

    void ResultBuilder::captureExpectedException( std::string const& expectedMessage ) {
        if( expectedMessage.empty() )
            captureExpectedException( Matchers::Impl::MatchAllOf<std::string>() );
        else
            captureExpectedException( Matchers::Equals( expectedMessage ) );
    }


    void ResultBuilder::captureExpectedException( Matchers::Impl::MatcherBase<std::string> const& matcher ) {

        assert( !isFalseTest( m_assertionInfo.resultDisposition ) );
        AssertionResultData data = m_data;
        data.resultType = ResultWas::Ok;
        data.reconstructedExpression = capturedExpressionWithSecondArgument(m_assertionInfo.capturedExpression, m_assertionInfo.secondArg);

        std::string actualMessage = Catch::translateActiveException();
        if( !matcher.match( actualMessage ) ) {
            data.resultType = ResultWas::ExpressionFailed;
            data.reconstructedExpression = actualMessage;
        }
        AssertionResult result( m_assertionInfo, data );
        handleResult( result );
    }

    void ResultBuilder::captureExpression() {
        AssertionResult result = build();
        handleResult( result );
    }

    void ResultBuilder::handleResult( AssertionResult const& result )
    {
        getResultCapture().assertionEnded( result );

        if( !result.isOk() ) {
            if( getCurrentContext().getConfig()->shouldDebugBreak() )
                m_shouldDebugBreak = true;
            if( getCurrentContext().getRunner()->aborting() || (m_assertionInfo.resultDisposition & ResultDisposition::Normal) )
                m_shouldThrow = true;
        }
    }

    void ResultBuilder::react() {
#if defined(CATCH_CONFIG_FAST_COMPILE)
        if (m_shouldDebugBreak) {
            ///////////////////////////////////////////////////////////////////
            // To inspect the state during test, you need to go one level up the callstack
            // To go back to the test and change execution, jump over the throw statement
            ///////////////////////////////////////////////////////////////////
            CATCH_BREAK_INTO_DEBUGGER();
        }
#endif
        if( m_shouldThrow )
            throw Catch::TestFailureException();
    }

    bool ResultBuilder::shouldDebugBreak() const { return m_shouldDebugBreak; }
    bool ResultBuilder::allowThrows() const { return getCurrentContext().getConfig()->allowThrows(); }

    AssertionResult ResultBuilder::build() const
    {
        return build( *this );
    }

    // CAVEAT: The returned AssertionResult stores a pointer to the argument expr,
    //         a temporary DecomposedExpression, which in turn holds references to
    //         operands, possibly temporary as well.
    //         It should immediately be passed to handleResult; if the expression
    //         needs to be reported, its string expansion must be composed before
    //         the temporaries are destroyed.
    AssertionResult ResultBuilder::build( DecomposedExpression const& expr ) const
    {
        assert( m_data.resultType != ResultWas::Unknown );
        AssertionResultData data = m_data;

        if(m_usedStream)
            data.message = m_stream().oss.str();
        data.decomposedExpression = &expr; // for lazy reconstruction
        return AssertionResult( m_assertionInfo, data );
    }

    void ResultBuilder::reconstructExpression( std::string& dest ) const {
        dest = capturedExpressionWithSecondArgument(m_assertionInfo.capturedExpression, m_assertionInfo.secondArg);
    }

    void ResultBuilder::setExceptionGuard() {
        m_guardException = true;
    }
    void ResultBuilder::unsetExceptionGuard() {
        m_guardException = false;
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RESULT_BUILDER_HPP_INCLUDED
