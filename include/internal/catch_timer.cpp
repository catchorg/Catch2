/*
 *  Created by Phil on 05/08/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_timer.h"

#include <chrono>

namespace Catch {

    auto getCurrentNanosecondsSinceEpoch() -> uint64_t {
        return std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() ).count();
    }

    auto estimateClockResolution() -> uint64_t {
        uint64_t sum = 0;
        static const uint64_t iterations = 1000000;

        for( std::size_t i = 0; i < iterations; ++i ) {

            uint64_t ticks;
            uint64_t baseTicks = getCurrentNanosecondsSinceEpoch();
            do {
                ticks = getCurrentNanosecondsSinceEpoch();
            }
            while( ticks == baseTicks );

            auto delta = ticks - baseTicks;
            sum += delta;
        }

        // We're just taking the mean, here. To do better we could take the std. dev and exclude outliers
        // - and potentially do more iterations if there's a high variance.
        return sum/iterations;
    }
    auto getEstimatedClockResolution() -> uint64_t {
        static auto s_resolution = estimateClockResolution();
        return s_resolution;
    }

    void Timer::start() {
       m_nanoseconds = getCurrentNanosecondsSinceEpoch();
    }
    auto Timer::getElapsedNanoseconds() const -> uint64_t {
        return getCurrentNanosecondsSinceEpoch() - m_nanoseconds;
    }
    auto Timer::getElapsedMicroseconds() const -> uint64_t {
        return getElapsedNanoseconds()/1000;
    }
    auto Timer::getElapsedMilliseconds() const -> unsigned int {
        return static_cast<unsigned int>(getElapsedMicroseconds()/1000);
    }
    auto Timer::getElapsedSeconds() const -> double {
        return getElapsedMicroseconds()/1000000.0;
    }


} // namespace Catch
