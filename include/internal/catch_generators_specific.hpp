/*
 *  Created by Martin on 15/6/2018.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_GENERATORS_SPECIFIC_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_GENERATORS_SPECIFIC_HPP_INCLUDED

#include "catch_context.h"
#include "catch_generators.hpp"
#include "catch_interfaces_config.h"
#include "catch_random_number_generator.h"

#include <random>

namespace Catch {
namespace Generators {

template <typename Float>
class RandomFloatingGenerator final : public IGenerator<Float> {
    Catch::SimplePcg32& m_rng;
    std::uniform_real_distribution<Float> m_dist;
    Float m_current_number;
public:

    RandomFloatingGenerator(Float a, Float b):
        m_rng(rng()),
        m_dist(a, b) {
        static_cast<void>(next());
    }

    Float const& get() const override {
        return m_current_number;
    }
    bool next() override {
        m_current_number = m_dist(m_rng);
        return true;
    }
};

template <typename Integer>
class RandomIntegerGenerator final : public IGenerator<Integer> {
    Catch::SimplePcg32& m_rng;
    std::uniform_int_distribution<Integer> m_dist;
    Integer m_current_number;
public:

    RandomIntegerGenerator(Integer a, Integer b):
        m_rng(rng()),
        m_dist(a, b) {
        static_cast<void>(next());
    }

    Integer const& get() const override {
        return m_current_number;
    }
    bool next() override {
        m_current_number = m_dist(m_rng);
        return true;
    }
};

// TODO: Ideally this would be also constrained against the various char types,
//       but I don't expect users to run into that in practice.
template <typename T>
typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value,
GeneratorWrapper<T>>::type
random(T a, T b) {
    return GeneratorWrapper<T>(
        pf::make_unique<RandomIntegerGenerator<T>>(a, b)
    );
}

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value,
GeneratorWrapper<T>>::type
random(T a, T b) {
    return GeneratorWrapper<T>(
        pf::make_unique<RandomFloatingGenerator<T>>(a, b)
    );
}


template <typename T>
class RangeGenerator final : public IGenerator<T> {
    T m_current;
    T m_end;
    T m_step;
    bool m_positive;

public:
    RangeGenerator(T const& start, T const& end, T const& step):
        m_current(start),
        m_end(end),
        m_step(step),
        m_positive(m_step > T(0))
    {
        assert(m_current != m_end && "Range start and end cannot be equal");
        assert(m_step != T(0) && "Step size cannot be zero");
        assert(((m_positive && m_current <= m_end) || (!m_positive && m_current >= m_end)) && "Step moves away from end");
    }

    RangeGenerator(T const& start, T const& end):
        RangeGenerator(start, end, (start < end) ? T(1) : T(-1))
    {}

    T const& get() const override {
        return m_current;
    }

    bool next() override {
        m_current += m_step;
        return (m_positive) ? (m_current < m_end) : (m_current > m_end);
    }
};

template <typename T>
GeneratorWrapper<T> range(T const& start, T const& end, T const& step) {
    static_assert(std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, "Type must be numeric");
    return GeneratorWrapper<T>(pf::make_unique<RangeGenerator<T>>(start, end, step));
}

template <typename T>
GeneratorWrapper<T> range(T const& start, T const& end) {
    static_assert(std::is_integral<T>::value && !std::is_same<T, bool>::value, "Type must be an integer");
    return GeneratorWrapper<T>(pf::make_unique<RangeGenerator<T>>(start, end));
}


template <typename T>
class IteratorGenerator final : public IGenerator<T> {
    static_assert(!std::is_same<T, bool>::value,
        "IteratorGenerator currently does not support bools"
        "because of std::vector<bool> specialization");

    std::vector<T> m_elems;
    size_t m_current = 0;
public:
    template <typename InputIterator, typename InputSentinel>
    IteratorGenerator(InputIterator first, InputSentinel last):m_elems(first, last) {
        if (m_elems.empty()) {
            Catch::throw_exception(GeneratorException("IteratorGenerator received no valid values"));
        }
    }

    T const& get() const override {
        return m_elems[m_current];
    }

    bool next() override {
        ++m_current;
        return m_current != m_elems.size();
    }
};

template <typename InputIterator,
          typename InputSentinel,
          typename ResultType = typename std::iterator_traits<InputIterator>::value_type>
GeneratorWrapper<ResultType> from_range(InputIterator from, InputSentinel to) {
    return GeneratorWrapper<ResultType>(pf::make_unique<IteratorGenerator<ResultType>>(from, to));
}

template <typename Container,
          typename ResultType = typename Container::value_type>
GeneratorWrapper<ResultType> from_range(Container const& cnt) {
    return GeneratorWrapper<ResultType>(pf::make_unique<IteratorGenerator<ResultType>>(cnt.begin(), cnt.end()));
}


} // namespace Generators
} // namespace Catch


#endif // TWOBLUECUBES_CATCH_GENERATORS_SPECIFIC_HPP_INCLUDED
