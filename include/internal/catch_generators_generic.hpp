/*
 *  Created by Martin on 23/2/2019.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_GENERATORS_GENERIC_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_GENERATORS_GENERIC_HPP_INCLUDED

#include "catch_generators.hpp"

namespace Catch {
namespace Generators {

    template <typename T>
    class TakeGenerator : public IGenerator<T> {
        GeneratorWrapper<T> m_generator;
        size_t m_returned = 0;
        size_t m_target;
    public:
        TakeGenerator(size_t target, GeneratorWrapper<T>&& generator):
            m_generator(std::move(generator)),
            m_target(target)
        {
            assert(target != 0 && "Empty generators are not allowed");
        }
        T const& get() const override {
            return m_generator.get();
        }
        bool next() override {
            ++m_returned;
            if (m_returned >= m_target) {
                return false;
            }

            const auto success = m_generator.next();
            // If the underlying generator does not contain enough values
            // then we cut short as well
            if (!success) {
                m_returned = m_target;
            }
            return success;
        }
    };

    template <typename T>
    GeneratorWrapper<T> take(size_t target, GeneratorWrapper<T>&& generator) {
        return GeneratorWrapper<T>(pf::make_unique<TakeGenerator<T>>(target, std::move(generator)));
    }


    template <typename T, typename Predicate>
    class FilterGenerator : public IGenerator<T> {
        GeneratorWrapper<T> m_generator;
        Predicate m_predicate;
    public:
        template <typename P = Predicate>
        FilterGenerator(P&& pred, GeneratorWrapper<T>&& generator):
            m_generator(std::move(generator)),
            m_predicate(std::forward<P>(pred))
        {
            if (!m_predicate(m_generator.get())) {
                // It might happen that there are no values that pass the
                // filter. In that case we throw an exception.
                auto has_initial_value = next();
                if (!has_initial_value) {
                    Catch::throw_exception(GeneratorException("No valid value found in filtered generator"));
                }
            }
        }

        T const& get() const override {
            return m_generator.get();
        }

        bool next() override {
            bool success = m_generator.next();
            if (!success) {
                return false;
            }
            while (!m_predicate(m_generator.get()) && (success = m_generator.next()) == true);
            return success;
        }
    };


    template <typename T, typename Predicate>
    GeneratorWrapper<T> filter(Predicate&& pred, GeneratorWrapper<T>&& generator) {
        return GeneratorWrapper<T>(std::unique_ptr<IGenerator<T>>(pf::make_unique<FilterGenerator<T, Predicate>>(std::forward<Predicate>(pred), std::move(generator))));
    }

    template <typename T>
    class RepeatGenerator : public IGenerator<T> {
        GeneratorWrapper<T> m_generator;
        mutable std::vector<T> m_returned;
        size_t m_target_repeats;
        size_t m_current_repeat = 0;
        size_t m_repeat_index = 0;
    public:
        RepeatGenerator(size_t repeats, GeneratorWrapper<T>&& generator):
            m_generator(std::move(generator)),
            m_target_repeats(repeats)
        {
            assert(m_target_repeats > 0 && "Repeat generator must repeat at least once");
        }

        T const& get() const override {
            if (m_current_repeat == 0) {
                m_returned.push_back(m_generator.get());
                return m_returned.back();
            }
            return m_returned[m_repeat_index];
        }

        bool next() override {
            // There are 2 basic cases:
            // 1) We are still reading the generator
            // 2) We are reading our own cache

            // In the first case, we need to poke the underlying generator.
            // If it happily moves, we are left in that state, otherwise it is time to start reading from our cache
            if (m_current_repeat == 0) {
                const auto success = m_generator.next();
                if (!success) {
                    ++m_current_repeat;
                }
                return m_current_repeat < m_target_repeats;
            }

            // In the second case, we need to move indices forward and check that we haven't run up against the end
            ++m_repeat_index;
            if (m_repeat_index == m_returned.size()) {
                m_repeat_index = 0;
                ++m_current_repeat;
            }
            return m_current_repeat < m_target_repeats;
        }
    };

    template <typename T>
    GeneratorWrapper<T> repeat(size_t repeats, GeneratorWrapper<T>&& generator) {
        return GeneratorWrapper<T>(pf::make_unique<RepeatGenerator<T>>(repeats, std::move(generator)));
    }

    template <typename T, typename U, typename Func>
    class MapGenerator : public IGenerator<T> {
        // TBD: provide static assert for mapping function, for friendly error message
        GeneratorWrapper<U> m_generator;
        Func m_function;
        // To avoid returning dangling reference, we have to save the values
        T m_cache;
    public:
        template <typename F2 = Func>
        MapGenerator(F2&& function, GeneratorWrapper<U>&& generator) :
            m_generator(std::move(generator)),
            m_function(std::forward<F2>(function)),
            m_cache(m_function(m_generator.get()))
        {}

        T const& get() const override {
            return m_cache;
        }
        bool next() override {
            const auto success = m_generator.next();
            if (success) {
                m_cache = m_function(m_generator.get());
            }
            return success;
        }
    };

#if defined(__cpp_lib_is_invocable) && __cpp_lib_is_invocable >= 201703
    // std::result_of is deprecated in C++17 and removed in C++20. Hence, it is
    // replaced with std::invoke_result here. Also *_t format is preferred over
    // typename *::type format.
    template <typename Func, typename U>
    using MapFunctionReturnType = std::remove_reference_t<std::remove_cv_t<std::invoke_result_t<Func, U>>>;
#else
    template <typename Func, typename U>
    using MapFunctionReturnType = typename std::remove_reference<typename std::remove_cv<typename std::result_of<Func(U)>::type>::type>::type;
#endif

    template <typename Func, typename U, typename T = MapFunctionReturnType<Func, U>>
    GeneratorWrapper<T> map(Func&& function, GeneratorWrapper<U>&& generator) {
        return GeneratorWrapper<T>(
            pf::make_unique<MapGenerator<T, U, Func>>(std::forward<Func>(function), std::move(generator))
        );
    }

    template <typename T, typename U, typename Func>
    GeneratorWrapper<T> map(Func&& function, GeneratorWrapper<U>&& generator) {
        return GeneratorWrapper<T>(
            pf::make_unique<MapGenerator<T, U, Func>>(std::forward<Func>(function), std::move(generator))
        );
    }

    template <typename T>
    class ChunkGenerator final : public IGenerator<std::vector<T>> {
        std::vector<T> m_chunk;
        size_t m_chunk_size;
        GeneratorWrapper<T> m_generator;
        bool m_used_up = false;
    public:
        ChunkGenerator(size_t size, GeneratorWrapper<T> generator) :
            m_chunk_size(size), m_generator(std::move(generator))
        {
            m_chunk.reserve(m_chunk_size);
            m_chunk.push_back(m_generator.get());
            for (size_t i = 1; i < m_chunk_size; ++i) {
                if (!m_generator.next()) {
                    Catch::throw_exception(GeneratorException("Not enough values to initialize the first chunk"));
                }
                m_chunk.push_back(m_generator.get());
            }
        }
        std::vector<T> const& get() const override {
            return m_chunk;
        }
        bool next() override {
            m_chunk.clear();
            for (size_t idx = 0; idx < m_chunk_size; ++idx) {
                if (!m_generator.next()) {
                    return false;
                }
                m_chunk.push_back(m_generator.get());
            }
            return true;
        }
    };

    template <typename T>
    GeneratorWrapper<std::vector<T>> chunk(size_t size, GeneratorWrapper<T>&& generator) {
        return GeneratorWrapper<std::vector<T>>(
            pf::make_unique<ChunkGenerator<T>>(size, std::move(generator))
        );
    }

} // namespace Generators
} // namespace Catch


#endif // TWOBLUECUBES_CATCH_GENERATORS_GENERIC_HPP_INCLUDED
