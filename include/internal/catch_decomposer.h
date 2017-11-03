/*
 *  Created by Phil Nash on 8/8/2017.
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_DECOMPOSER_H_INCLUDED
#define TWOBLUECUBES_CATCH_DECOMPOSER_H_INCLUDED

#include "catch_tostring.h"
#include "catch_stringref.h"

#include <ostream>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4389) // '==' : signed/unsigned mismatch
#pragma warning(disable:4018) // more "signed/unsigned mismatch"
#pragma warning(disable:4312) // Converting int to T* using reinterpret_cast (issue on x64 platform)
#pragma warning(disable:4180) // qualifier applied to function type has no meaning
#endif

namespace Catch {

    struct ITransientExpression {
        virtual auto isBinaryExpression() const -> bool = 0;
        virtual auto getResult() const -> bool = 0;
        virtual void streamReconstructedExpression( std::ostream &os ) const = 0;

        // We don't actually need a virtual destructore, but many static analysers
        // complain if it's not here :-(
        virtual ~ITransientExpression();
    };

    void formatReconstructedExpression( std::ostream &os, std::string const& lhs, StringRef op, std::string const& rhs );

    template<typename LhsT, typename RhsT>
    class BinaryExpr  : public ITransientExpression {
        bool m_result;
        LhsT m_lhs;
        StringRef m_op;
        RhsT m_rhs;

        auto isBinaryExpression() const -> bool override { return true; }
        auto getResult() const -> bool override { return m_result; }

        void streamReconstructedExpression( std::ostream &os ) const override {
            formatReconstructedExpression
                    ( os, Catch::Detail::stringify( m_lhs ), m_op, Catch::Detail::stringify( m_rhs ) );
        }

    public:
        BinaryExpr( bool comparisonResult, LhsT lhs, StringRef op, RhsT rhs )
        :   m_result( comparisonResult ),
            m_lhs( lhs ),
            m_op( op ),
            m_rhs( rhs )
        {}
    };

    template<typename LhsT>
    class UnaryExpr : public ITransientExpression {
        LhsT m_lhs;

        auto isBinaryExpression() const -> bool override { return false; }
        auto getResult() const -> bool override { return m_lhs ? true : false; }

        void streamReconstructedExpression( std::ostream &os ) const override {
            os << Catch::Detail::stringify( m_lhs );
        }

    public:
        UnaryExpr( LhsT lhs ) : m_lhs( lhs ) {}
    };


    // Specialised comparison functions to handle equality comparisons between ints and pointers (NULL deduces as an int)
    template<typename LhsT, typename RhsT>
    auto compareEqual( LhsT const& lhs, RhsT const& rhs ) -> bool { return lhs == rhs; };
    template<typename T>
    auto compareEqual( T* const& lhs, int rhs ) -> bool { return lhs == reinterpret_cast<void const*>( rhs ); }
    template<typename T>
    auto compareEqual( T* const& lhs, long rhs ) -> bool { return lhs == reinterpret_cast<void const*>( rhs ); }
    template<typename T>
    auto compareEqual( int lhs, T* const& rhs ) -> bool { return reinterpret_cast<void const*>( lhs ) == rhs; }
    template<typename T>
    auto compareEqual( long lhs, T* const& rhs ) -> bool { return reinterpret_cast<void const*>( lhs ) == rhs; }

    template<typename LhsT, typename RhsT>
    auto compareNotEqual( LhsT const& lhs, RhsT&& rhs ) -> bool { return lhs != rhs; };
    template<typename T>
    auto compareNotEqual( T* const& lhs, int rhs ) -> bool { return lhs != reinterpret_cast<void const*>( rhs ); }
    template<typename T>
    auto compareNotEqual( T* const& lhs, long rhs ) -> bool { return lhs != reinterpret_cast<void const*>( rhs ); }
    template<typename T>
    auto compareNotEqual( int lhs, T* const& rhs ) -> bool { return reinterpret_cast<void const*>( lhs ) != rhs; }
    template<typename T>
    auto compareNotEqual( long lhs, T* const& rhs ) -> bool { return reinterpret_cast<void const*>( lhs ) != rhs; }


    template<typename LhsT>
    class ExprLhs {
        LhsT m_lhs;
    public:
        ExprLhs( LhsT lhs ) : m_lhs( lhs ) {}

        template<typename RhsT>
        auto operator == ( RhsT const& rhs ) -> BinaryExpr<LhsT, RhsT const&> const {
            return BinaryExpr<LhsT, RhsT const&>( compareEqual( m_lhs, rhs ), m_lhs, "==", rhs );
        }
        auto operator == ( bool rhs ) -> BinaryExpr<LhsT, bool> const {
            return BinaryExpr<LhsT, bool>( m_lhs == rhs, m_lhs, "==", rhs );
        }

        template<typename RhsT>
        auto operator != ( RhsT const& rhs ) -> BinaryExpr<LhsT, RhsT const&> const {
            return BinaryExpr<LhsT, RhsT const&>( compareNotEqual( m_lhs, rhs ), m_lhs, "!=", rhs );
        }
        auto operator != ( bool rhs ) -> BinaryExpr<LhsT, bool> const {
            return BinaryExpr<LhsT, bool>( m_lhs != rhs, m_lhs, "!=", rhs );
        }

        template<typename RhsT>
        auto operator > ( RhsT const& rhs ) -> BinaryExpr<LhsT, RhsT const&> const {
            return BinaryExpr<LhsT, RhsT const&>( m_lhs > rhs, m_lhs, ">", rhs );
        }
        template<typename RhsT>
        auto operator < ( RhsT const& rhs ) -> BinaryExpr<LhsT, RhsT const&> const {
            return BinaryExpr<LhsT, RhsT const&>( m_lhs < rhs, m_lhs, "<", rhs );
        }
        template<typename RhsT>
        auto operator >= ( RhsT const& rhs ) -> BinaryExpr<LhsT, RhsT const&> const {
            return BinaryExpr<LhsT, RhsT const&>( m_lhs >= rhs, m_lhs, ">=", rhs );
        }
        template<typename RhsT>
        auto operator <= ( RhsT const& rhs ) -> BinaryExpr<LhsT, RhsT const&> const {
            return BinaryExpr<LhsT, RhsT const&>( m_lhs <= rhs, m_lhs, "<=", rhs );
        }

        auto makeUnaryExpr() const -> UnaryExpr<LhsT> {
            return UnaryExpr<LhsT>( m_lhs );
        }
    };

    void handleExpression( ITransientExpression const& expr );

    template<typename T>
    void handleExpression( ExprLhs<T> const& expr ) {
        handleExpression( expr.makeUnaryExpr() );
    }

    struct Decomposer {
        template<typename T>
        auto operator <= ( T const& lhs ) -> ExprLhs<T const&> {
            return ExprLhs<T const&>( lhs );
        }
        auto operator <=( bool value ) -> ExprLhs<bool> {
            return ExprLhs<bool>( value );
        }
    };

} // end namespace Catch

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // TWOBLUECUBES_CATCH_DECOMPOSER_H_INCLUDED
