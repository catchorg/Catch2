/*
 *  Created by Martin on 19/07/2017.
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_approx.hpp"

#include <limits>

namespace Catch {
namespace Detail {

    double max(double lhs, double rhs) {
        if (lhs < rhs) {
            return rhs;
        }
        return lhs;
    }

    Approx::Approx ( double value )
    :   m_epsilon( std::numeric_limits<float>::epsilon()*100 ),
        m_margin( 0.0 ),
        m_scale( 1.0 ),
        m_value( value )
    {}

    Approx Approx::custom() {
        return Approx( 0 );
    }

    std::string Approx::toString() const {
        std::ostringstream oss;
        oss << "Approx( " << ::Catch::Detail::stringify( m_value ) << " )";
        return oss.str();
    }

} // end namespace Detail

std::string StringMaker<Catch::Detail::Approx>::convert(Catch::Detail::Approx const& value) {
    return value.toString();
}

} // end namespace Catch
