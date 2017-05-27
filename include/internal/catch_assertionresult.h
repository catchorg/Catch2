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
#include "catch_result_type.h"

namespace Catch {

    struct STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison;

    struct DecomposedExpression
    {
        virtual ~DecomposedExpression() {}
        virtual bool isBinaryExpression() const {
            return false;
        }
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

    private:
        DecomposedExpression& operator = (DecomposedExpression const&);
    };

    struct AssertionInfo
    {
        AssertionInfo() {}
        AssertionInfo(  std::string const& _macroName,
                        SourceLineInfo const& _lineInfo,
                        std::string const& _capturedExpression,
                        ResultDisposition::Flags _resultDisposition );

        std::string macroName;
        SourceLineInfo lineInfo;
        std::string capturedExpression;
        ResultDisposition::Flags resultDisposition;
    };

    struct AssertionResultData
    {
        AssertionResultData() : decomposedExpression( CATCH_NULL )
                              , resultType( ResultWas::Unknown )
                              , negated( false )
                              , parenthesized( false ) {}

        void negate( bool parenthesize ) {
            negated = !negated;
            parenthesized = parenthesize;
            if( resultType == ResultWas::Ok )
                resultType = ResultWas::ExpressionFailed;
            else if( resultType == ResultWas::ExpressionFailed )
                resultType = ResultWas::Ok;
        }

        std::string const& reconstructExpression() const {
            if( decomposedExpression != CATCH_NULL ) {
                decomposedExpression->reconstructExpression( reconstructedExpression );
                if( parenthesized ) {
                    reconstructedExpression.insert( 0, 1, '(' );
                    reconstructedExpression.append( 1, ')' );
                }
                if( negated ) {
                    reconstructedExpression.insert( 0, 1, '!' );
                }
                decomposedExpression = CATCH_NULL;
            }
            return reconstructedExpression;
        }

        mutable DecomposedExpression const* decomposedExpression;
        mutable std::string reconstructedExpression;
        std::string message;
        ResultWas::OfType resultType;
        bool negated;
        bool parenthesized;
    };

    class AssertionResult {
    public:
        AssertionResult();
        AssertionResult( AssertionInfo const& info, AssertionResultData const& data );
        ~AssertionResult();
#  ifdef CATCH_CONFIG_CPP11_GENERATED_METHODS
         AssertionResult( AssertionResult const& )              = default;
         AssertionResult( AssertionResult && )                  = default;
         AssertionResult& operator = ( AssertionResult const& ) = default;
         AssertionResult& operator = ( AssertionResult && )     = default;
#  endif

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

    protected:
        AssertionInfo m_info;
        AssertionResultData m_resultData;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_ASSERTIONRESULT_H_INCLUDED
