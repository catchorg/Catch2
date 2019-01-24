/*
 *  Created by Phil on 05/08/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_timer.h"
#include <vector>
#include <chrono>

static const uint64_t nanosecondsInSecond = 1000000000;
static const uint64_t STD_DEVIATION_THRESHOLD = 100;

namespace Catch {

    auto getCurrentNanosecondsSinceEpoch() -> uint64_t {
        return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    }

    namespace {
        auto estimateClockResolution() -> uint64_t {
            uint64_t result = 0;
            bool std_deviation_is_stable = false;
            do {
                uint64_t sum = 0;
                std::vector<uint64_t> deltas;
                static const uint64_t iterations = 1000000;

                auto startTime = getCurrentNanosecondsSinceEpoch();

                for (std::size_t i = 0; i < iterations; ++i) {

                    uint64_t ticks;
                    uint64_t baseTicks = getCurrentNanosecondsSinceEpoch();
                    do {
                        ticks = getCurrentNanosecondsSinceEpoch();
                    } while (ticks == baseTicks);

                    auto delta = ticks - baseTicks;
                    deltas.push_back(delta);
                    sum += delta;
                // If we have been calibrating for over 3 seconds -- the clock
                // is terrible and we should move on.
                // TBD: How to signal that the measured resolution is probably wrong?
                if (ticks > startTime + 3 * nanosecondsInSecond) {
                    return sum / ( i + 1u );
                }

                // We're just taking the mean, here. To do better we could take the std. dev and exclude outliers
                // - and potentially do more iterations if there's a high variance.
                result = sum / iterations;
                auto squared_difference_sum = 0;
                for (std::size_t i = 0; i < iterations; ++i) {
                    squared_difference_sum += (deltas[i] - result) * (deltas[i] - result);
                }
                auto std_deviation = squared_difference_sum / iterations;
                if (std_deviation < STD_DEVIATION_THRESHOLD) {
                    std_deviation_is_stable = true;
                }
            } while (!std_deviation_is_stable);
            return result;
        }
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
        return getElapsedNanoseconds() / 1000;
    }

    auto Timer::getElapsedMilliseconds() const -> unsigned int {
        return static_cast<unsigned int>(getElapsedMicroseconds() / 1000);
    }

    auto Timer::getElapsedSeconds() const -> double {
        return getElapsedMicroseconds() / 1000000.0;
    }


} // namespace Catch
