// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <catch2/matchers/catch_matchers_container_properties.hpp>

#include <catch2/internal/catch_stream.hpp>

namespace Catch {
namespace Matchers {

    std::string IsEmptyMatcher::describe() const {
        return "is empty";
    }

    std::string HasSizeMatcher::describe() const {
        ReusableStringStream sstr;
        sstr << "has size == " << m_target_size;
        return sstr.str();
    }

    IsEmptyMatcher IsEmpty() {
        return {};
    }

    HasSizeMatcher SizeIs(std::size_t sz) {
        return HasSizeMatcher{ sz };
    }

} // end namespace Matchers
} // end namespace Catch
