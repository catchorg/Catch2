//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_MATCHERS_ELEMENTS_ARE_HPP_INCLUDED
#define CATCH_MATCHERS_ELEMENTS_ARE_HPP_INCLUDED

#include <catch2/matchers/catch_matchers_templated.hpp>

#include <algorithm>
#include <utility>

namespace Catch {
namespace Matchers {

    template<typename RangeLike>
    class ElementsAreElementsMatcher final : public MatcherGenericBase {
        RangeLike m_range;
	public:
		ElementsAreElementsMatcher(RangeLike range):
		    m_range(std::move(range))
		{}

		bool match(RangeLike rng) const {
		    return std::equal(m_range.begin(), m_range.end(), rng.begin());
		}

		std::string describe() const override {
		    return "elements are " + Catch::Detail::stringify(m_range);
		}
    };

    template<typename RangeLike>
    class UnorderedElementsAreElementsMatcher final : public MatcherGenericBase {
        RangeLike m_range;
	public:
		UnorderedElementsAreElementsMatcher(RangeLike range):
		    m_range(std::move(range))
		{}

		bool match(RangeLike rng) const {
		    return std::is_permutation(m_range.begin(), m_range.end(), rng.begin());
		}

		std::string describe() const override {
		    return "unordered elements are " + ::Catch::Detail::stringify( m_range );
		}
    };

    //! Creates a matcher that checks if all elements in a range are equal to all elements in another range
    template<typename RangeLike>
    ElementsAreElementsMatcher<RangeLike> ElementsAre(RangeLike rng) {
        return { std::forward<RangeLike>(rng) };
    }

    //! Creates a matcher that checks if all elements in a range are equal to all elements in another range in some permutation
    template<typename RangeLike>
    UnorderedElementsAreElementsMatcher<RangeLike> UnorderedElementsAre(RangeLike rng) {
        return { std::forward<RangeLike>(rng) };
    }

} // namespace Matchers
} // namespace Catch

#endif // CATCH_MATCHERS_ELEMENTS_ARE_HPP_INCLUDED
