/*
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <catch2/matchers/catch_matchers.hpp>

namespace Catch {

    namespace Matchers {

        std::string MatcherUntypedBase::toString() const {
            if (m_cachedToString.empty())
                m_cachedToString = describe();
            return m_cachedToString;
        }

        MatcherUntypedBase::~MatcherUntypedBase() = default;

    } // namespace Matchers

} // namespace Catch
