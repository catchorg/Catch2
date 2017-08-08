/*
 *  Created by Phil on 28/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_ASSERTIONRESULT_H_INCLUDED
#define TWOBLUECUBES_CATCH_ASSERTIONRESULT_H_INCLUDED

#include <string>
#include "catch_assertioninfo.h"
#include "catch_result_type.h"
#include "catch_common.h"
#include "catch_stringref.h"
#include "catch_assertionhandler.h"

namespace Catch {

    struct STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison;

    struct DecomposedExpression
    {
        DecomposedExpression() = default;
        DecomposedExpression( DecomposedExpression const& ) = default;
        DecomposedExpression& operator = ( DecomposedExpression const& ) = delete;

        virtual ~DecomposedExpression() = default;
        virtual bool isBinaryExpression() const;
        virtual void reconstructExpression( std::string& dest ) const = 0;

        // Only simple binary comparisons can be decomposed.
        // If more complex check is required then wrap sub-expressions in parentheses.
        template<typename T> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator + ( T const& );
        template<typename T> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator - ( T const& );
        template<typename T> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator * ( T const& );
        template<typename T> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator / ( T const& );
        template<typename T> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator % ( T const& );
        template<typename T> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator && ( T const& );
        template<typename T> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator || ( T const& );
    };
}

namespace Catch {

    struct AssertionResultData
    {
        AssertionResultData() = delete;

        // !TBD We won't need this constructor once the deprecated fields are removed
        AssertionResultData( ResultWas::OfType _resultType, LazyExpression const& _lazyExpression )
        :   resultType( _resultType ),
            lazyExpression( _lazyExpression )
        {}

        ResultWas::OfType resultType = ResultWas::Unknown;
        std::string message;

        LazyExpression lazyExpression;

        // deprecated:
        bool negated = false;
        bool parenthesized = false;
        void negate( bool parenthesize );
        std::string reconstructExpression() const;
        mutable DecomposedExpression const* decomposedExpression = nullptr;
        mutable std::string reconstructedExpression;
    };

    class AssertionResult {
    public:
        AssertionResult() = delete;
        AssertionResult( AssertionInfo const& info, AssertionResultData const& data );

        bool isOk() const;
        bool succeeded() const;
        ResultWas::OfType getResultType() const;
        bool hasExpression() const;
        bool hasMessage() const;
        std::string getExpression() const;
        std::string getExpressionInMacro() const;
        bool hasExpandedExpression() const;
        std::string getExpandedExpression() const;
        std::string getMessage() const;
        SourceLineInfo getSourceInfo() const;
        std::string getTestMacroName() const;
        void discardDecomposedExpression() const;
        void expandDecomposedExpression() const;

    //protected:
        AssertionInfo m_info;
        AssertionResultData m_resultData;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_ASSERTIONRESULT_H_INCLUDED
