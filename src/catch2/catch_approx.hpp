
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_APPROX_HPP_INCLUDED
#define CATCH_APPROX_HPP_INCLUDED

#include <catch2/catch_tostring.hpp>

#include <type_traits>
#include <limits>

namespace Catch {

    class Approx {
    private:
        // Performs equivalent check of std::fabs(lhs - rhs) <= margin
        // But without the subtraction to allow for INFINITY in comparison
        constexpr bool marginComparison (double lhs, double rhs, double margin) const {
            return (lhs + margin >= rhs) && (rhs + margin >= lhs);
        }

        constexpr double fabs(double value) const {
            return (value < 0.0) ? -value : value;
        }

        constexpr bool isinf(double value) const {
            return value == std::numeric_limits<double>::infinity() || value == -std::numeric_limits<double>::infinity();
        }

        constexpr bool equalityComparisonImpl(double other) const {
            // First try with fixed margin, then compute margin based on epsilon, scale and Approx's value
            // Thanks to Richard Harris for his help refining the scaled margin value
            return marginComparison(m_value, other, m_margin)
                || marginComparison(m_value, other, m_epsilon * (m_scale + fabs(isinf(m_value)? 0 : m_value)));
        }

        // Sets and validates the new epsilon (0 < epsilon < 1)
        constexpr void setEpsilon(double epsilon) {
            if(epsilon < 0)
                throw std::domain_error("Invalid Approx::epsilon. Approx::epsilon has to be in [0, 1]");
            m_epsilon = epsilon;
        }

        // Sets and validates the new margin (margin >= 0)
        constexpr void setMargin(double margin) {
            if(margin < 0)
                throw std::domain_error("Invalid Approx::margin. Approx::Margin has to be non-negative.");
            m_margin = margin;
        }

    public:
        constexpr inline explicit Approx ( double value )
        :   m_epsilon( static_cast<double>(std::numeric_limits<float>::epsilon())*100. ),
            m_margin( 0.0 ),
            m_scale( 0.0 ),
            m_value( value )
        {}

        static constexpr Approx custom() {
            return Approx( 0.0 );
        }

        constexpr Approx operator-() const {
            auto temp(*this);
            temp.m_value = -temp.m_value;
            return temp;
        }

        template <typename T, typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        constexpr Approx operator()( T const& value ) const {
            Approx approx( static_cast<double>(value) );
            approx.m_epsilon = m_epsilon;
            approx.m_margin = m_margin;
            approx.m_scale = m_scale;
            return approx;
        }

        template <typename T, typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        constexpr inline explicit Approx( T const& value ): Approx(static_cast<double>(value))
        {}


        template <typename T, typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        friend constexpr bool operator == ( const T& lhs, Approx const& rhs ) {
            auto lhs_v = static_cast<double>(lhs);
            return rhs.equalityComparisonImpl(lhs_v);
        }

        template <typename T, typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        friend constexpr bool operator == ( Approx const& lhs, const T& rhs ) {
            return operator==( rhs, lhs );
        }

        template <typename T, typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        friend constexpr bool operator != ( T const& lhs, Approx const& rhs ) {
            return !operator==( lhs, rhs );
        }

        template <typename T, typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        friend constexpr bool operator != ( Approx const& lhs, T const& rhs ) {
            return !operator==( rhs, lhs );
        }

        template <typename T, typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        friend constexpr bool operator <= ( T const& lhs, Approx const& rhs ) {
            return static_cast<double>(lhs) < rhs.m_value || lhs == rhs;
        }

        template <typename T, typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        friend constexpr bool operator <= ( Approx const& lhs, T const& rhs ) {
            return lhs.m_value < static_cast<double>(rhs) || lhs == rhs;
        }

        template <typename T, typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        friend constexpr bool operator >= ( T const& lhs, Approx const& rhs ) {
            return static_cast<double>(lhs) > rhs.m_value || lhs == rhs;
        }

        template <typename T, typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        friend constexpr bool operator >= ( Approx const& lhs, T const& rhs ) {
            return lhs.m_value > static_cast<double>(rhs) || lhs == rhs;
        }

        template <typename T, typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        constexpr Approx& epsilon( T const& newEpsilon ) {
            const auto epsilonAsDouble = static_cast<double>(newEpsilon);
            setEpsilon(epsilonAsDouble);
            return *this;
        }

        template <typename T, typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        constexpr Approx& margin( T const& newMargin ) {
            const auto marginAsDouble = static_cast<double>(newMargin);
            setMargin(marginAsDouble);
            return *this;
        }

        template <typename T, typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        constexpr Approx& scale( T const& newScale ) {
            m_scale = static_cast<double>(newScale);
            return *this;
        }

        std::string toString() const;

    private:
        double m_epsilon;
        double m_margin;
        double m_scale;
        double m_value;
    };

namespace literals {
    constexpr Approx operator ""_a(long double val) {
        return Approx(val);
    }
    constexpr Approx operator ""_a(unsigned long long val) {
        return Approx(val);
    }
} // end namespace literals

template<>
struct StringMaker<Catch::Approx> {
    static std::string convert(Catch::Approx const& value);
};

} // end namespace Catch

#endif // CATCH_APPROX_HPP_INCLUDED
