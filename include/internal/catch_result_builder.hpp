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
#include "catch_expressionresult_builder.h"
#include "catch_context.h"
#include "catch_common.h"
#include "catch_interfaces_config.h"
#include "catch_interfaces_runner.h"
#include "catch_interfaces_capture.h"
#include "catch_interfaces_registry_hub.h"


namespace Catch {

    ExpressionResultBuilder& getResultBuilder( ResultBuilder* rb ) {
        return rb->m_resultBuilder;
    }

    ResultBuilder::ResultBuilder(   char const* macroName,
                                    SourceLineInfo const& lineInfo,
                                    char const* capturedExpression,
                                    ResultDisposition::Flags resultDisposition )
        :   m_assertionInfo( macroName, lineInfo, capturedExpression, resultDisposition ),
            m_resultBuilder( this ),
            m_shouldDebugBreak( false ),
            m_shouldThrow( false )
        {}

    void ResultBuilder::useActiveException( ResultDisposition::Flags resultDisposition ) {
        m_assertionInfo.resultDisposition = resultDisposition;
        m_resultBuilder << Catch::translateActiveException();
        captureResult( ResultWas::ThrewException );
    }

    void ResultBuilder::captureResult( ResultWas::OfType resultType ) {
        m_resultBuilder.setResultType( resultType );
        captureExpression();
    }

    void ResultBuilder::captureExpression() {
        AssertionResult result = m_resultBuilder.buildResult( m_assertionInfo );
        getResultCapture().assertionEnded( result );

        if( !result.isOk() ) {
            if( getCurrentContext().getConfig()->shouldDebugBreak() )
                m_shouldDebugBreak = true;
            if( getCurrentContext().getRunner()->aborting() || m_assertionInfo.resultDisposition == ResultDisposition::Normal )
                m_shouldThrow = true;
        }
    }
    void ResultBuilder::react() {
        if( m_shouldThrow )
            throw Catch::TestFailureException();
    }

    bool ResultBuilder::shouldDebugBreak() const { return m_shouldDebugBreak; }
    bool ResultBuilder::allowThrows() const { return getCurrentContext().getConfig()->allowThrows(); }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RESULT_BUILDER_HPP_INCLUDED
