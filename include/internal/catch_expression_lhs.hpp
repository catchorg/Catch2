/*
 *  Created by Phil on 11/5/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_EXPRESSION_LHS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_EXPRESSION_LHS_HPP_INCLUDED

#include "catch_result_builder.h"
#include "catch_evaluate.hpp"
#include "catch_tostring.h"

namespace Catch {

template<typename LhsT, Internal::Operator Op, typename RhsT>
class BinaryExpression;

template<typename ArgT, typename MatcherT>
class MatchExpression;

// Wraps the LHS of an expression and overloads comparison operators
// for also capturing those and RHS (if any)
template<typename T>
class ExpressionLhs : public DecomposedExpression {
public:
    ExpressionLhs( ResultBuilder& rb, T lhs ) : m_rb( rb ), m_lhs( lhs ), m_truthy(false) {}

    ExpressionLhs& operator = ( const ExpressionLhs& );

    template<typename RhsT>
    BinaryExpression<T, Internal::IsEqualTo, RhsT const&>
    operator == ( RhsT const& rhs ) {
        return captureExpression<Internal::IsEqualTo>( rhs );
    }

    template<typename RhsT>
    BinaryExpression<T, Internal::IsNotEqualTo, RhsT const&>
    operator != ( RhsT const& rhs ) {
        return captureExpression<Internal::IsNotEqualTo>( rhs );
    }

    template<typename RhsT>
    BinaryExpression<T, Internal::IsLessThan, RhsT const&>
    operator < ( RhsT const& rhs ) {
        return captureExpression<Internal::IsLessThan>( rhs );
    }

    template<typename RhsT>
    BinaryExpression<T, Internal::IsGreaterThan, RhsT const&>
    operator > ( RhsT const& rhs ) {
        return captureExpression<Internal::IsGreaterThan>( rhs );
    }

    template<typename RhsT>
    BinaryExpression<T, Internal::IsLessThanOrEqualTo, RhsT const&>
    operator <= ( RhsT const& rhs ) {
        return captureExpression<Internal::IsLessThanOrEqualTo>( rhs );
    }

    template<typename RhsT>
    BinaryExpression<T, Internal::IsGreaterThanOrEqualTo, RhsT const&>
    operator >= ( RhsT const& rhs ) {
        return captureExpression<Internal::IsGreaterThanOrEqualTo>( rhs );
    }

    BinaryExpression<T, Internal::IsEqualTo, bool> operator == ( bool rhs ) {
        return captureExpression<Internal::IsEqualTo>( rhs );
    }

    BinaryExpression<T, Internal::IsNotEqualTo, bool> operator != ( bool rhs ) {
        return captureExpression<Internal::IsNotEqualTo>( rhs );
    }

    void endExpression() {
        m_truthy = m_lhs ? true : false;
        m_rb
            .setResultType( m_truthy )
            .endExpression( *this );
    }

    virtual void reconstructExpression( std::string& dest ) const CATCH_OVERRIDE {
        dest = Catch::toString( m_lhs );
    }

private:
    template<Internal::Operator Op, typename RhsT>
    BinaryExpression<T, Op, RhsT&> captureExpression( RhsT& rhs ) const {
        return BinaryExpression<T, Op, RhsT&>( m_rb, m_lhs, rhs );
    }

    template<Internal::Operator Op>
    BinaryExpression<T, Op, bool> captureExpression( bool rhs ) const {
        return BinaryExpression<T, Op, bool>( m_rb, m_lhs, rhs );
    }

private:
    ResultBuilder& m_rb;
    T m_lhs;
    bool m_truthy;
};

template<typename LhsT, Internal::Operator Op, typename RhsT>
class BinaryExpression : public DecomposedExpression {
public:
    BinaryExpression( ResultBuilder& rb, LhsT lhs, RhsT rhs )
        : m_rb( rb ), m_lhs( lhs ), m_rhs( rhs ) {}

    BinaryExpression& operator = ( BinaryExpression& );

    void endExpression() const {
        m_rb
            .setResultType(  Internal::Evaluator<LhsT, RhsT, Op>::evaluate( m_lhs, m_rhs ) )
            .endExpression( *this );
    }

    virtual bool isBinaryExpression() const CATCH_OVERRIDE {
        return true;
    }

    virtual void reconstructExpression( std::string& dest ) const CATCH_OVERRIDE {
        std::string lhs = Catch::toString( m_lhs );
        std::string rhs = Catch::toString( m_rhs );
        char delim = lhs.size() + rhs.size() < 40 &&
                     lhs.find('\n') == std::string::npos &&
                     rhs.find('\n') == std::string::npos ? ' ' : '\n';
        dest.reserve( 7 + lhs.size() + rhs.size() );
                   // 2 for spaces around operator
                   // 2 for operator
                   // 2 for parentheses (conditionally added later)
                   // 1 for negation (conditionally added later)
        dest = lhs;
        dest += delim;
        dest += Internal::OperatorTraits<Op>::getName();
        dest += delim;
        dest += rhs;
    }

private:
    ResultBuilder& m_rb;
    LhsT m_lhs;
    RhsT m_rhs;
};

template<typename ArgT, typename MatcherT>
class MatchExpression : public DecomposedExpression {
public:
    MatchExpression( ArgT arg, MatcherT matcher, char const* matcherString )
        : m_arg( arg ), m_matcher( matcher ), m_matcherString( matcherString ) {}

    virtual bool isBinaryExpression() const CATCH_OVERRIDE {
        return true;
    }

    virtual void reconstructExpression( std::string& dest ) const CATCH_OVERRIDE {
        std::string matcherAsString = m_matcher.toString();
        dest = Catch::toString( m_arg );
        dest += ' ';
        if( matcherAsString == Detail::unprintableString )
            dest += m_matcherString;
        else
            dest += matcherAsString;
    }

private:
    ArgT m_arg;
    MatcherT m_matcher;
    char const* m_matcherString;
};

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_EXPRESSION_LHS_HPP_INCLUDED
