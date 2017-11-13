/*
 *  Created by Phil Nash on 21/02/2017.
 *  Copyright (c) 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_MATCHERS_VECTOR_H_INCLUDED
#define TWOBLUECUBES_CATCH_MATCHERS_VECTOR_H_INCLUDED

#include "catch_matchers.h"

namespace Catch {
namespace Matchers {

    namespace Vector {

        template<typename T>
        struct ContainsElementMatcher : MatcherBase<std::vector<T>> {

            ContainsElementMatcher(T const &comparator) : m_comparator( comparator) {}

            bool match(std::vector<T> const &v) const override {
                for (auto const& el : v) {
                    if (el == m_comparator) {
                        return true;
                    }
                }
                return false;
            }

            std::string describe() const override {
                return "Contains: " + ::Catch::Detail::stringify( m_comparator );
            }

            T const& m_comparator;
        };

        template<typename T>
        struct ContainsMatcher : MatcherBase<std::vector<T>> {

            ContainsMatcher(std::vector<T> const &comparator) : m_comparator( comparator ) {}

            bool match(std::vector<T> const &v) const override {
                // !TBD: see note in EqualsMatcher
                if (m_comparator.size() > v.size())
                    return false;
                for (auto const& comparator : m_comparator) {
                    auto present = false;
                    for (const auto& el : v) {
                        if (el == comparator) {
                            present = true;
                            break;
                        }
                    }
                    if (!present) {
                        return false;
                    }
                }
                return true;
            }
            std::string describe() const override {
                return "Contains: " + ::Catch::Detail::stringify( m_comparator );
            }

            std::vector<T> const& m_comparator;
        };

        template<typename T>
        struct EqualsMatcher : MatcherBase<std::vector<T>> {

            EqualsMatcher(std::vector<T> const &comparator) : m_comparator( comparator ) {}

            bool match(std::vector<T> const &v) const override {
                // !TBD: This currently works if all elements can be compared using !=
                // - a more general approach would be via a compare template that defaults
                // to using !=. but could be specialised for, e.g. std::vector<T> etc
                // - then just call that directly
                if (m_comparator.size() != v.size())
                    return false;
                for (std::size_t i = 0; i < v.size(); ++i)
                    if (m_comparator[i] != v[i])
                        return false;
                return true;
            }
            std::string describe() const override {
                return "Equals: " + ::Catch::Detail::stringify( m_comparator );
            }
            std::vector<T> const& m_comparator;
        };

    } // namespace Vector

    // The following functions create the actual matcher objects.
    // This allows the types to be inferred

    template<typename T>
    Vector::ContainsMatcher<T> Contains( std::vector<T> const& comparator ) {
        return Vector::ContainsMatcher<T>( comparator );
    }

    template<typename T>
    Vector::ContainsElementMatcher<T> VectorContains( T const& comparator ) {
        return Vector::ContainsElementMatcher<T>( comparator );
    }

    template<typename T>
    Vector::EqualsMatcher<T> Equals( std::vector<T> const& comparator ) {
        return Vector::EqualsMatcher<T>( comparator );
    }

} // namespace Matchers
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_MATCHERS_VECTOR_H_INCLUDED
