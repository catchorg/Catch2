
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/catch_approx.hpp>
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_reusable_string_stream.hpp>

#include <cmath>

namespace Catch {

std::string Approx::toString() const {
    ReusableStringStream rss;
    rss << "Approx( " << ::Catch::Detail::stringify( m_value ) << " )";
    return rss.str();
}

std::string StringMaker<Catch::Approx>::convert(Catch::Approx const& value) {
    return value.toString();
}

} // end namespace Catch
