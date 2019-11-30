/*
 *  Created by Martin Hořeňovský on 13/10/2019.
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_matchers_exception.hpp"


namespace Catch {
namespace Matchers {
namespace Exception {

bool ExceptionMessageMatcher::match(std::exception const& ex) const {
    return ex.what() == m_message;
}

std::string ExceptionMessageMatcher::describe() const {
    return "exception message matches \"" + m_message + "\"";
}

}
Exception::ExceptionMessageMatcher Message(std::string const& message) {
    return Exception::ExceptionMessageMatcher(message);
}

// namespace Exception
} // namespace Matchers
} // namespace Catch
