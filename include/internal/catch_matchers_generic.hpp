/*
 *  Created by Martin Hořeňovský on 03/04/2017.
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_MATCHERS_GENERIC_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_MATCHERS_GENERIC_HPP_INCLUDED

#include "catch_common.h"
#include "catch_matchers.h"

#include <functional>
#include <string>

namespace Catch {
namespace Matchers {
namespace Generic {

namespace Detail {
    std::string finalizeDescription(const std::string& desc);
}

template <typename T>
class PredicateMatcher : public MatcherBase<T> {
    std::function<bool(T const&)> m_predicate;
    std::string m_description;
public:

    PredicateMatcher(std::function<bool(T const&)> const& elem, std::string const& descr)
        :m_predicate(std::move(elem)),
        m_description(Detail::finalizeDescription(descr))
    {}

    bool match( T const& item ) const override {
        return m_predicate(item);
    }

    std::string describe() const override {
        return m_description;
    }
};

} // namespace Generic

    // The following functions create the actual matcher objects.
    // The user has to explicitly specify type to the function, because
    // infering std::function<bool(T const&)> is hard (but possible) and
    // requires a lot of TMP.
    template<typename T>
    Generic::PredicateMatcher<T> Predicate(std::function<bool(T const&)> const& predicate, std::string const& description = "") {
        return Generic::PredicateMatcher<T>(predicate, description);
    }

} // namespace Matchers
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_MATCHERS_GENERIC_HPP_INCLUDED
