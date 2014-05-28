/*
 *  Created by Phil on 28/5/2014.
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RESULT_BUILDER_H_INCLUDED
#define TWOBLUECUBES_CATCH_RESULT_BUILDER_H_INCLUDED

#include "catch_expression_lhs.hpp"
#include "catch_expressionresult_builder.h"
#include "catch_common.h"

namespace Catch {

    struct TestFailureException{};

    struct ResultBuilder {
        ResultBuilder(  char const* macroName,
                        SourceLineInfo const& lineInfo,
                        char const* capturedExpression,
                        ResultDisposition::Flags resultDisposition );

        template<typename T>
        ExpressionLhs<T const&> operator->* ( T const& operand ) {
            return ExpressionLhs<T const&>( *this, operand );
        }

        ExpressionLhs<bool> operator->* ( bool value ) {
            return ExpressionLhs<bool>( *this, value );
        }

        void useActiveException( ResultDisposition::Flags resultDisposition = ResultDisposition::Normal );
        void captureResult( ResultWas::OfType resultType );
        void captureExpression();
        void react();
        bool shouldDebugBreak() const;
        bool allowThrows() const;

        AssertionInfo m_assertionInfo;
        ExpressionResultBuilder m_resultBuilder;
        bool m_shouldDebugBreak;
        bool m_shouldThrow;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RESULT_BUILDER_H_INCLUDED
