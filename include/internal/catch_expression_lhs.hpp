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

    template<typename RhsT>
    BinaryExpression<T, Internal::IsEqualTo, RhsT const&>
    operator == ( RhsT const& rhs ) const {
        return captureExpression<Internal::IsEqualTo>( rhs );
    }

    template<typename RhsT>
    BinaryExpression<T, Internal::IsNotEqualTo, RhsT const&>
    operator != ( RhsT const& rhs ) const {
        return captureExpression<Internal::IsNotEqualTo>( rhs );
    }

    template<typename RhsT>
    BinaryExpression<T, Internal::IsLessThan, RhsT const&>
    operator < ( RhsT const& rhs ) const {
        return captureExpression<Internal::IsLessThan>( rhs );
    }

    template<typename RhsT>
    BinaryExpression<T, Internal::IsGreaterThan, RhsT const&>
    operator > ( RhsT const& rhs ) const {
        return captureExpression<Internal::IsGreaterThan>( rhs );
    }

    template<typename RhsT>
    BinaryExpression<T, Internal::IsLessThanOrEqualTo, RhsT const&>
    operator <= ( RhsT const& rhs ) const {
        return captureExpression<Internal::IsLessThanOrEqualTo>( rhs );
    }

    template<typename RhsT>
    BinaryExpression<T, Internal::IsGreaterThanOrEqualTo, RhsT const&>
    operator >= ( RhsT const& rhs ) const {
        return captureExpression<Internal::IsGreaterThanOrEqualTo>( rhs );
    }

    BinaryExpression<T, Internal::IsEqualTo, bool> operator == ( bool rhs ) const {
        return captureExpression<Internal::IsEqualTo>( rhs );
    }

    BinaryExpression<T, Internal::IsNotEqualTo, bool> operator != ( bool rhs ) const {
        return captureExpression<Internal::IsNotEqualTo>( rhs );
    }

    void endExpression() {
        m_truthy = m_lhs ? true : false;
        m_rb
            .setResultType( m_truthy )
            .endExpression( *this );
    }

    virtual std::string reconstructExpression() const CATCH_OVERRIDE {
        return Catch::toString( m_truthy );
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

    void endExpression() const {
        m_rb
            .setResultType( Internal::compare<Op>( m_lhs, m_rhs ) )
            .endExpression( *this );
    }

    virtual bool isBinaryExpression() const CATCH_OVERRIDE {
        return true;
    }

    virtual std::string reconstructExpression() const CATCH_OVERRIDE {
        return reconstructExpressionImpl
                (Catch::toString( m_lhs ),
                 Catch::toString( m_rhs ),
                 Internal::OperatorTraits<Op>::getName() );
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

    virtual std::string reconstructExpression() const CATCH_OVERRIDE {
        std::string matcherAsString = m_matcher.toString();
        std::string dest = Catch::toString( m_arg ) + " ";
        if( matcherAsString == Detail::unprintableString )
            dest += m_matcherString;
        else
            dest += matcherAsString;
        return dest;
    }

private:
    ArgT m_arg;
    MatcherT m_matcher;
    char const* m_matcherString;
};

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_EXPRESSION_LHS_HPP_INCLUDED
