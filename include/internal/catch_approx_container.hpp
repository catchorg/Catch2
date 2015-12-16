/*
 *  Created by Phil on 28/04/2011.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_APPROX_CONTAINER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_APPROX_CONTAINER_HPP_INCLUDED

#include "catch_approx.hpp"

namespace Catch {
namespace Detail {

    template<template <typename T, typename Alloc = std::allocator<T> > class Container = std::vector>
    class ApproxContainer : public Approx {
        typedef ApproxContainer<Container> my_t;
    public:

        template<typename T>
        ApproxContainer (const Container<T> & c)
        :   Approx( 0. ),
            m_values( c.begin(), c.end() )
        {}

        ApproxContainer( ApproxContainer const& other )
        :   Approx(other),
            m_values( other.m_values )
        {}

        static ApproxContainer custom() {
            return ApproxContainer( Container<double>() );
        }

        template<typename T>
        ApproxContainer operator()(const Container<T> & c) {
            ApproxContainer approx( c );
            approx.epsilon( m_epsilon );
            approx.scale( m_scale );
            return approx;
        }

        template<typename T>
        friend bool operator == ( const Container<T> & lhs, my_t const& rhs ) {
            bool success = rhs.m_values.size() == lhs.size();
            typename Container<T>::const_iterator lhs_it = lhs.begin();
            typename Container<double>::const_iterator values_it = rhs.m_values.begin();
            for (; lhs_it != lhs.end() && success; ++lhs_it, ++values_it) {
                success &= rhs.compare(*lhs_it, *values_it);
            }
            return success;
        }

        template<typename T>
        friend bool operator == ( my_t const& lhs, Container<T> rhs ) {
            return operator==( rhs, lhs );
        }

        template<typename T>
        friend bool operator != ( Container<T> lhs, my_t const& rhs ) {
            return !operator==( lhs, rhs );
        }

        template<typename T>
        friend bool operator != ( my_t const& lhs, std::vector<T> rhs ) {
            return !operator==( rhs, lhs );
        }

        std::string toString() const {
            std::ostringstream oss;
            oss << "Approx( " << Catch::toString( m_values) << " )";
            return oss.str();
        }

        my_t& epsilon( double newEpsilon ) {
            m_epsilon = newEpsilon;
            return *this;
        }

        my_t& scale( double newScale ) {
            m_scale = newScale;
            return *this;
        }

    protected:
        Container<double> m_values;
    };

    template<template <typename T, typename Alloc> class Container>
    std::ostream& operator<<(std::ostream& os, const ApproxContainer<Container>& rhs) {
        return os << rhs.toString();
    }
}

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_APPROX_CONTAINER_HPP_INCLUDED
