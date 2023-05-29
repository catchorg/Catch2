
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// Adapted from donated nonius code.

#ifndef CATCH_ESTIMATE_CLOCK_HPP_INCLUDED
#define CATCH_ESTIMATE_CLOCK_HPP_INCLUDED

#include <catch2/benchmark/catch_clock.hpp>
#include <catch2/benchmark/catch_environment.hpp>
#include <catch2/benchmark/detail/catch_stats.hpp>
#include <catch2/benchmark/detail/catch_measure.hpp>
#include <catch2/benchmark/detail/catch_run_for_at_least.hpp>
#include <catch2/benchmark/catch_clock.hpp>
#include <catch2/internal/catch_unique_ptr.hpp>

#include <algorithm>
#include <vector>
#include <cmath>

namespace Catch {
    namespace Benchmark {
        namespace Detail {
            template <typename Clock>
            std::vector<double> resolution(int k) {
                std::vector<TimePoint<Clock>> times;
                times.reserve(static_cast<size_t>(k + 1));
                for ( int i = 0; i < k + 1; ++i ) {
                    times.push_back( Clock::now() );
                }

                std::vector<double> deltas;
                deltas.reserve(static_cast<size_t>(k));
                for ( size_t idx = 1; idx < times.size(); ++idx ) {
                    deltas.push_back( static_cast<double>(
                        ( times[idx] - times[idx - 1] ).count() ) );
                }

                return deltas;
            }

            constexpr auto warmup_iterations = 10000;
            constexpr auto warmup_time = std::chrono::milliseconds(100);
            constexpr auto minimum_ticks = 1000;
            constexpr auto warmup_seed = 10000;
            constexpr auto clock_resolution_estimation_time = std::chrono::milliseconds(500);
            constexpr auto clock_cost_estimation_time_limit = std::chrono::seconds(1);
            constexpr auto clock_cost_estimation_tick_limit = 100000;
            constexpr auto clock_cost_estimation_time = std::chrono::milliseconds(10);
            constexpr auto clock_cost_estimation_iterations = 10000;

            template <typename Clock>
            int warmup() {
                return run_for_at_least<Clock>(std::chrono::duration_cast<ClockDuration<Clock>>(warmup_time), warmup_seed, &resolution<Clock>)
                    .iterations;
            }
            template <typename Clock>
            EnvironmentEstimate<FloatDuration<Clock>> estimate_clock_resolution(int iterations) {
                auto r = run_for_at_least<Clock>(std::chrono::duration_cast<ClockDuration<Clock>>(clock_resolution_estimation_time), iterations, &resolution<Clock>)
                    .result;
                return {
                    FloatDuration<Clock>(mean(r.begin(), r.end())),
                    classify_outliers(r.begin(), r.end()),
                };
            }
            template <typename Clock>
            EnvironmentEstimate<FloatDuration<Clock>> estimate_clock_cost(FloatDuration<Clock> resolution) {
                auto time_limit = (std::min)(
                    resolution * clock_cost_estimation_tick_limit,
                    FloatDuration<Clock>(clock_cost_estimation_time_limit));
                auto time_clock = [](int k) {
                    return Detail::measure<Clock>([k] {
                        for (int i = 0; i < k; ++i) {
                            volatile auto ignored = Clock::now();
                            (void)ignored;
                        }
                    }).elapsed;
                };
                time_clock(1);
                int iters = clock_cost_estimation_iterations;
                auto&& r = run_for_at_least<Clock>(std::chrono::duration_cast<ClockDuration<Clock>>(clock_cost_estimation_time), iters, time_clock);
                std::vector<double> times;
                int nsamples = static_cast<int>(std::ceil(time_limit / r.elapsed));
                times.reserve(static_cast<size_t>(nsamples));
                for ( int s = 0; s < nsamples; ++s ) {
                    times.push_back( static_cast<double>(
                        ( time_clock( r.iterations ) / r.iterations )
                            .count() ) );
                }
                return {
                    FloatDuration<Clock>(mean(times.begin(), times.end())),
                    classify_outliers(times.begin(), times.end()),
                };
            }

            template <typename Clock>
            Environment<FloatDuration<Clock>> measure_environment() {
#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif
                static Catch::Detail::unique_ptr<Environment<FloatDuration<Clock>>> env;
#if defined(__clang__)
#    pragma clang diagnostic pop
#endif
                if (env) {
                    return *env;
                }

                auto iters = Detail::warmup<Clock>();
                auto resolution = Detail::estimate_clock_resolution<Clock>(iters);
                auto cost = Detail::estimate_clock_cost<Clock>(resolution.mean);

                env = Catch::Detail::make_unique<Environment<FloatDuration<Clock>>>( Environment<FloatDuration<Clock>>{resolution, cost} );
                return *env;
            }
        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch

#endif // CATCH_ESTIMATE_CLOCK_HPP_INCLUDED
