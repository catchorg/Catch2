
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// Adapted from donated nonius code.

#ifndef CATCH_OPTIMIZER_HPP_INCLUDED
#define CATCH_OPTIMIZER_HPP_INCLUDED

#include <catch2/internal/catch_compiler_detections.hpp>

#if defined(CATCH_COMPILER_MSC)
#   include <atomic> // atomic_thread_fence
#endif

#include <catch2/internal/catch_move_and_forward.hpp>

#include <type_traits>

namespace Catch {
    namespace Benchmark {
#if defined(CATCH_COMPILER_GCC) || defined(CATCH_COMPILER_CLANG)
        template <typename T>
        inline void keep_memory(T* p) {
            asm volatile("" : : "g"(p) : "memory");
        }
        inline void keep_memory() {
            asm volatile("" : : : "memory");
        }

        namespace Detail {
            inline void optimizer_barrier() { keep_memory(); }
        } // namespace Detail
#elif defined(CATCH_COMPILER_MSC)

#pragma optimize("", off)
        template <typename T>
        inline void keep_memory(T* p) {
            // thanks @milleniumbug
            *reinterpret_cast<char volatile*>(p) = *reinterpret_cast<char const volatile*>(p);
        }
        // TODO equivalent keep_memory()
#pragma optimize("", on)

        namespace Detail {
            inline void optimizer_barrier() {
                std::atomic_thread_fence(std::memory_order_seq_cst);
            }
        } // namespace Detail

#endif

        template <typename T>
        inline void deoptimize_value(T&& x) {
            keep_memory(&x);
        }

        template <typename Fn, typename... Args>
        inline auto invoke_deoptimized(Fn&& fn, Args&&... args) -> std::enable_if_t<!std::is_same<void, decltype(fn(args...))>::value> {
            deoptimize_value(CATCH_FORWARD(fn) (CATCH_FORWARD(args)...));
        }

        template <typename Fn, typename... Args>
        inline auto invoke_deoptimized(Fn&& fn, Args&&... args) -> std::enable_if_t<std::is_same<void, decltype(fn(args...))>::value> {
            CATCH_FORWARD(fn) (CATCH_FORWARD(args)...);
        }
    } // namespace Benchmark
} // namespace Catch

#endif // CATCH_OPTIMIZER_HPP_INCLUDED
