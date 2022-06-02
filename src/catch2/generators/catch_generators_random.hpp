
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_GENERATORS_RANDOM_HPP_INCLUDED
#define CATCH_GENERATORS_RANDOM_HPP_INCLUDED

#include <catch2/internal/catch_context.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/internal/catch_random_number_generator.hpp>

#include <random>

namespace Catch {
namespace Generators {
namespace Detail {
    // Returns a suitable seed for a random floating generator based off
    // the primary internal rng. It does so by taking current value from
    // the rng and returning it as the seed.
    std::uint32_t getSeed();
}

template <typename Float>
class RandomFloatingGenerator final : public IGenerator<Float> {
    Catch::SimplePcg32 m_rng;
    std::uniform_real_distribution<Float> m_dist;
    Float m_current_number;
public:
    RandomFloatingGenerator( Float a, Float b, std::uint32_t seed ):
        m_rng(seed),
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
    Catch::SimplePcg32 m_rng;
    std::uniform_int_distribution<Integer> m_dist;
    Integer m_current_number;
public:
    RandomIntegerGenerator( Integer a, Integer b, std::uint32_t seed ):
        m_rng(seed),
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

template <typename T>
std::enable_if_t<std::is_integral<T>::value, GeneratorWrapper<T>>
random(T a, T b) {
    static_assert(
        !std::is_same<T, char>::value &&
        !std::is_same<T, int8_t>::value &&
        !std::is_same<T, uint8_t>::value &&
        !std::is_same<T, signed char>::value &&
        !std::is_same<T, unsigned char>::value &&
        !std::is_same<T, bool>::value,
        "The requested type is not supported by the underlying random distributions from std" );
    return GeneratorWrapper<T>(
        Catch::Detail::make_unique<RandomIntegerGenerator<T>>(a, b, Detail::getSeed())
    );
}

template <typename T>
std::enable_if_t<std::is_floating_point<T>::value,
GeneratorWrapper<T>>
random(T a, T b) {
    return GeneratorWrapper<T>(
        Catch::Detail::make_unique<RandomFloatingGenerator<T>>(a, b, Detail::getSeed())
    );
}


} // namespace Generators
} // namespace Catch


#endif // CATCH_GENERATORS_RANDOM_HPP_INCLUDED
