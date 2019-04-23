/*
 *  Created by Joachim on 16/04/2019.
 *  Adapted from donated nonius code.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// Clocks

#ifndef TWOBLUECUBES_CATCH_CLOCK_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CLOCK_HPP_INCLUDED

#include <chrono>
#include <ratio>

namespace Catch {
    namespace Benchmark {
        template <unsigned Num, unsigned Den = 1>
        using ratio = std::ratio<Num, Den>;
        using milli = ratio<1, 1000>;
        using micro = ratio<1, 1000000>;
        using nano = ratio<1, 1000000000>;

        template <typename Clock>
        using ClockDuration = typename Clock::duration;
        template <typename Clock>
        using FloatDuration = std::chrono::duration<double, typename Clock::period>;

        template <typename Clock>
        using TimePoint = typename Clock::time_point;

        using default_clock = std::chrono::high_resolution_clock;

        template <typename Clock>
        struct now {
            TimePoint<Clock> operator()() const {
                return Clock::now();
            }
        };

        using fp_seconds = std::chrono::duration<double, ratio<1>>;
    } // namespace Benchmark
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_CLOCK_HPP_INCLUDED
