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
#include "catch_meta.hpp"

#include <string>
#include <utility>

namespace Catch {
namespace Matchers {
namespace Generic {

namespace Detail {
    std::string finalizeDescription(const std::string& desc);
}

template <typename T, typename Predicate>
class PredicateMatcher : public MatcherBase<T> {
    Predicate m_predicate;
    std::string m_description;
public:

    PredicateMatcher(Predicate&& elem, std::string const& descr)
        :m_predicate(std::forward<Predicate>(elem)),
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
    // inferring std::function<bool(T const&)> is hard (but possible) and
    // requires a lot of TMP.
    template<typename T, typename Pred>
    Generic::PredicateMatcher<T, Pred> Predicate(Pred&& predicate, std::string const& description = "") {
        static_assert(is_callable<Pred(T)>::value, "Predicate not callable with argument T");
        static_assert(std::is_same<bool, FunctionReturnType<Pred, T>>::value, "Predicate does not return bool");
        return Generic::PredicateMatcher<T, Pred>(std::forward<Pred>(predicate), description);
    }

} // namespace Matchers
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_MATCHERS_GENERIC_HPP_INCLUDED
