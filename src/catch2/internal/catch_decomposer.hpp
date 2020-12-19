
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_DECOMPOSER_HPP_INCLUDED
#define CATCH_DECOMPOSER_HPP_INCLUDED

#include <catch2/catch_tostring.hpp>
#include <catch2/internal/catch_stringref.hpp>
#include <catch2/internal/catch_meta.hpp>

#include <iosfwd>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4389) // '==' : signed/unsigned mismatch
#pragma warning(disable:4018) // more "signed/unsigned mismatch"
#pragma warning(disable:4312) // Converting int to T* using reinterpret_cast (issue on x64 platform)
#pragma warning(disable:4180) // qualifier applied to function type has no meaning
#pragma warning(disable:4800) // Forcing result to true or false
#endif

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wsign-compare"
#elif defined __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsign-compare"
#endif

namespace Catch {

    struct ITransientExpression {
        auto isBinaryExpression() const -> bool { return m_isBinaryExpression; }
        auto getResult() const -> bool { return m_result; }
        virtual void streamReconstructedExpression( std::ostream &os ) const = 0;

        ITransientExpression( bool isBinaryExpression, bool result )
        :   m_isBinaryExpression( isBinaryExpression ),
            m_result( result )
        {}

        ITransientExpression() = default;
        ITransientExpression(ITransientExpression const&) = default;
        ITransientExpression& operator=(ITransientExpression const&) = default;

        // We don't actually need a virtual destructor, but many static analysers
        // complain if it's not here :-(
        virtual ~ITransientExpression(); // = default;

        bool m_isBinaryExpression;
        bool m_result;
        friend std::ostream& operator<<(std::ostream& out, ITransientExpression const& expr) {
            expr.streamReconstructedExpression(out);
            return out;
        }
    };

    void formatReconstructedExpression( std::ostream &os, std::string const& lhs, StringRef op, std::string const& rhs );

    template<typename LhsT, typename RhsT>
    class BinaryExpr  : public ITransientExpression {
        LhsT m_lhs;
        StringRef m_op;
        RhsT m_rhs;

        void streamReconstructedExpression( std::ostream &os ) const override {
            formatReconstructedExpression
                    ( os, Catch::Detail::stringify( m_lhs ), m_op, Catch::Detail::stringify( m_rhs ) );
        }

    public:
        BinaryExpr( bool comparisonResult, LhsT lhs, StringRef op, RhsT rhs )
        :   ITransientExpression{ true, comparisonResult },
            m_lhs( lhs ),
            m_op( op ),
            m_rhs( rhs )
        {}

        template<typename T>
        auto operator && ( T ) const -> BinaryExpr<LhsT, RhsT const&> const {
            static_assert(always_false<T>::value,
            "chained comparisons are not supported inside assertions, "
            "wrap the expression inside parentheses, or decompose it");
        }

        template<typename T>
        auto operator || ( T ) const -> BinaryExpr<LhsT, RhsT const&> const {
            static_assert(always_false<T>::value,
            "chained comparisons are not supported inside assertions, "
            "wrap the expression inside parentheses, or decompose it");
        }

        template<typename T>
        auto operator == ( T ) const -> BinaryExpr<LhsT, RhsT const&> const {
            static_assert(always_false<T>::value,
            "chained comparisons are not supported inside assertions, "
            "wrap the expression inside parentheses, or decompose it");
        }

        template<typename T>
        auto operator != ( T ) const -> BinaryExpr<LhsT, RhsT const&> const {
            static_assert(always_false<T>::value,
            "chained comparisons are not supported inside assertions, "
            "wrap the expression inside parentheses, or decompose it");
        }

        template<typename T>
        auto operator > ( T ) const -> BinaryExpr<LhsT, RhsT const&> const {
            static_assert(always_false<T>::value,
            "chained comparisons are not supported inside assertions, "
            "wrap the expression inside parentheses, or decompose it");
        }

        template<typename T>
        auto operator < ( T ) const -> BinaryExpr<LhsT, RhsT const&> const {
            static_assert(always_false<T>::value,
            "chained comparisons are not supported inside assertions, "
            "wrap the expression inside parentheses, or decompose it");
        }

        template<typename T>
        auto operator >= ( T ) const -> BinaryExpr<LhsT, RhsT const&> const {
            static_assert(always_false<T>::value,
            "chained comparisons are not supported inside assertions, "
            "wrap the expression inside parentheses, or decompose it");
        }

        template<typename T>
        auto operator <= ( T ) const -> BinaryExpr<LhsT, RhsT const&> const {
            static_assert(always_false<T>::value,
            "chained comparisons are not supported inside assertions, "
            "wrap the expression inside parentheses, or decompose it");
        }
    };

    template<typename LhsT>
    class UnaryExpr : public ITransientExpression {
        LhsT m_lhs;

        void streamReconstructedExpression( std::ostream &os ) const override {
            os << Catch::Detail::stringify( m_lhs );
        }

    public:
        explicit UnaryExpr( LhsT lhs )
        :   ITransientExpression{ false, static_cast<bool>(lhs) },
            m_lhs( lhs )
        {}
    };


    // Specialised comparison functions to handle equality comparisons between ints and pointers (NULL deduces as an int)
    template<typename LhsT, typename RhsT>
    auto compareEqual( LhsT const& lhs, RhsT const& rhs ) -> bool { return static_cast<bool>(lhs == rhs); }
    template<typename T>
    auto compareEqual( T* const& lhs, int rhs ) -> bool { return lhs == reinterpret_cast<void const*>( rhs ); }
    template<typename T>
    auto compareEqual( T* const& lhs, long rhs ) -> bool { return lhs == reinterpret_cast<void const*>( rhs ); }
    template<typename T>
    auto compareEqual( int lhs, T* const& rhs ) -> bool { return reinterpret_cast<void const*>( lhs ) == rhs; }
    template<typename T>
    auto compareEqual( long lhs, T* const& rhs ) -> bool { return reinterpret_cast<void const*>( lhs ) == rhs; }

    template<typename LhsT, typename RhsT>
    auto compareNotEqual( LhsT const& lhs, RhsT&& rhs ) -> bool { return static_cast<bool>(lhs != rhs); }
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
        explicit ExprLhs( LhsT lhs ) : m_lhs( lhs ) {}

        template<typename RhsT, std::enable_if_t<!std::is_arithmetic<std::remove_reference_t<RhsT>>::value, int> = 0>
        friend auto operator == ( ExprLhs && lhs, RhsT && rhs ) -> BinaryExpr<LhsT, RhsT const&> {
            return { compareEqual( lhs.m_lhs, rhs ), lhs.m_lhs, "=="_sr, rhs };
        }
        template<typename RhsT, std::enable_if_t<std::is_arithmetic<RhsT>::value, int> = 0>
        friend auto operator == ( ExprLhs && lhs, RhsT rhs ) -> BinaryExpr<LhsT, RhsT> {
            return { compareEqual( lhs.m_lhs, rhs ), lhs.m_lhs, "=="_sr, rhs };
        }

        template<typename RhsT, std::enable_if_t<!std::is_arithmetic<std::remove_reference_t<RhsT>>::value, int> = 0>
        friend auto operator != ( ExprLhs && lhs, RhsT && rhs ) -> BinaryExpr<LhsT, RhsT const&> {
            return { compareNotEqual( lhs.m_lhs, rhs ), lhs.m_lhs, "!="_sr, rhs };
        }
        template<typename RhsT, std::enable_if_t<std::is_arithmetic<RhsT>::value, int> = 0>
        friend auto operator != ( ExprLhs && lhs, RhsT rhs ) -> BinaryExpr<LhsT, RhsT> {
            return { compareNotEqual( lhs.m_lhs, rhs ), lhs.m_lhs, "!="_sr, rhs };
        }

    #define CATCH_INTERNAL_DEFINE_EXPRESSION_OPERATOR(op) \
        template<typename RhsT, std::enable_if_t<!std::is_arithmetic<std::remove_reference_t<RhsT>>::value, int> = 0> \
        friend auto operator op ( ExprLhs && lhs, RhsT && rhs ) -> BinaryExpr<LhsT, RhsT const&> { \
            return { static_cast<bool>(lhs.m_lhs op rhs), lhs.m_lhs, #op##_sr, rhs }; \
        } \
        template<typename RhsT, std::enable_if_t<std::is_arithmetic<RhsT>::value, int> = 0> \
        friend auto operator op ( ExprLhs && lhs, RhsT rhs ) -> BinaryExpr<LhsT, RhsT> { \
            return { static_cast<bool>(lhs.m_lhs op rhs), lhs.m_lhs, #op##_sr, rhs }; \
        }

        CATCH_INTERNAL_DEFINE_EXPRESSION_OPERATOR(<)
        CATCH_INTERNAL_DEFINE_EXPRESSION_OPERATOR(>)
        CATCH_INTERNAL_DEFINE_EXPRESSION_OPERATOR(<=)
        CATCH_INTERNAL_DEFINE_EXPRESSION_OPERATOR(>=)
        CATCH_INTERNAL_DEFINE_EXPRESSION_OPERATOR(|)
        CATCH_INTERNAL_DEFINE_EXPRESSION_OPERATOR(&)
        CATCH_INTERNAL_DEFINE_EXPRESSION_OPERATOR(^)

    #undef CATCH_INTERNAL_DEFINE_EXPRESSION_OPERATOR

        template<typename RhsT>
        friend auto operator && ( ExprLhs &&, RhsT && ) -> BinaryExpr<LhsT, RhsT const&> {
            static_assert(always_false<RhsT>::value,
            "operator&& is not supported inside assertions, "
            "wrap the expression inside parentheses, or decompose it");
        }

        template<typename RhsT>
        friend auto operator || ( ExprLhs &&, RhsT && ) -> BinaryExpr<LhsT, RhsT const&> {
            static_assert(always_false<RhsT>::value,
            "operator|| is not supported inside assertions, "
            "wrap the expression inside parentheses, or decompose it");
        }

        auto makeUnaryExpr() const -> UnaryExpr<LhsT> {
            return UnaryExpr<LhsT>{ m_lhs };
        }
    };

    struct Decomposer {
        template<typename T, std::enable_if_t<!std::is_arithmetic<std::remove_reference_t<T>>::value, int> = 0>
        friend auto operator <= ( Decomposer &&, T && lhs ) -> ExprLhs<T const&> {
            return ExprLhs<const T&>{ lhs };
        }

        template<typename T, std::enable_if_t<std::is_arithmetic<T>::value, int> = 0>
        friend auto operator <= ( Decomposer &&, T value ) -> ExprLhs<T> {
            return ExprLhs<T>{ value };
        }
    };

} // end namespace Catch

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined __GNUC__
#  pragma GCC diagnostic pop
#endif

#endif // CATCH_DECOMPOSER_HPP_INCLUDED
