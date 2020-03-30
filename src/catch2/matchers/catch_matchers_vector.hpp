/*
 *  Created by Phil Nash on 21/02/2017.
 *  Copyright (c) 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_MATCHERS_VECTOR_H_INCLUDED
#define TWOBLUECUBES_CATCH_MATCHERS_VECTOR_H_INCLUDED

#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/catch_approx.hpp>

#include <algorithm>

namespace Catch {
namespace Matchers {

    template<typename T>
    struct ContainsElementMatcher final : MatcherBase<std::vector<T>> {

        ContainsElementMatcher(T const& comparator):
            m_comparator(comparator)
        {}

        bool match(std::vector<T> const& v) const override {
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
    struct ContainsMatcher final : MatcherBase<std::vector<T>> {

        ContainsMatcher(std::vector<T> const& comparator):
            m_comparator(comparator)
        {}

        bool match(std::vector<T> const& v) const override {
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
    struct EqualsMatcher final : MatcherBase<std::vector<T>> {

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

    template<typename T>
    struct ApproxMatcher final : MatcherBase<std::vector<T>> {

        ApproxMatcher(std::vector<T> const& comparator) : m_comparator( comparator ) {}

        bool match(std::vector<T> const &v) const override {
            if (m_comparator.size() != v.size())
                return false;
            for (std::size_t i = 0; i < v.size(); ++i)
                if (m_comparator[i] != approx(v[i]))
                    return false;
            return true;
        }
        std::string describe() const override {
            return "is approx: " + ::Catch::Detail::stringify( m_comparator );
        }
        template <typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        ApproxMatcher& epsilon( T const& newEpsilon ) {
            approx.epsilon(newEpsilon);
            return *this;
        }
        template <typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        ApproxMatcher& margin( T const& newMargin ) {
            approx.margin(newMargin);
            return *this;
        }
        template <typename = std::enable_if_t<std::is_constructible<double, T>::value>>
        ApproxMatcher& scale( T const& newScale ) {
            approx.scale(newScale);
            return *this;
        }

        std::vector<T> const& m_comparator;
        mutable Catch::Approx approx = Catch::Approx::custom();
    };

    template<typename T>
    struct UnorderedEqualsMatcher final : MatcherBase<std::vector<T>> {
        UnorderedEqualsMatcher(std::vector<T> const& target) : m_target(target) {}
        bool match(std::vector<T> const& vec) const override {
            // Note: This is a reimplementation of std::is_permutation,
            //       because I don't want to include <algorithm> inside the common path
            if (m_target.size() != vec.size()) {
                return false;
            }
            return std::is_permutation(m_target.begin(), m_target.end(), vec.begin());
        }

        std::string describe() const override {
            return "UnorderedEquals: " + ::Catch::Detail::stringify(m_target);
        }
    private:
        std::vector<T> const& m_target;
    };


    // The following functions create the actual matcher objects.
    // This allows the types to be inferred


    //! Creates a matcher that matches vectors that contain all elements in `comparator`
    template<typename T>
    ContainsMatcher<T> Contains( std::vector<T> const& comparator ) {
        return ContainsMatcher<T>( comparator );
    }

    //! Creates a matcher that matches vectors that contain `comparator` as an element
    template<typename T>
    ContainsElementMatcher<T> VectorContains( T const& comparator ) {
        return ContainsElementMatcher<T>( comparator );
    }

    //! Creates a matcher that matches vectors that are exactly equal to `comparator`
    template<typename T>
    EqualsMatcher<T> Equals( std::vector<T> const& comparator ) {
        return EqualsMatcher<T>( comparator );
    }

    //! Creates a matcher that matches vectors that `comparator` as an element
    template<typename T>
    ApproxMatcher<T> Approx( std::vector<T> const& comparator ) {
        return ApproxMatcher<T>( comparator );
    }

    //! Creates a matcher that matches vectors that is equal to `target` modulo permutation
    template<typename T>
    UnorderedEqualsMatcher<T> UnorderedEquals(std::vector<T> const& target) {
        return UnorderedEqualsMatcher<T>(target);
    }

} // namespace Matchers
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_MATCHERS_VECTOR_H_INCLUDED
