
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/benchmark/catch_benchmark.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>

namespace Catch {
    namespace Benchmark {
        namespace Detail {

            Environment measure_environment_default() {
                return Detail::measure_environment<default_clock>();
            }

            ExecutionPlan prepare_default( const IConfig& cfg,
                                           Environment env,
                                           BenchmarkFunction&& fun ) {
                // This mirrors Benchmark::prepare<default_clock>(), but with the
                // clock fixed so it is instantiated once here in the library.
                auto min_time = env.clock_resolution.mean * Detail::minimum_ticks;
                auto run_time = std::max(
                    min_time,
                    std::chrono::duration_cast<decltype( min_time )>(
                        cfg.benchmarkWarmupTime() ) );
                auto&& test = Detail::run_for_at_least<default_clock>(
                    std::chrono::duration_cast<IDuration>( run_time ), 1, fun );
                int new_iters = static_cast<int>(
                    std::ceil( min_time * test.iterations / test.elapsed ) );
                return { new_iters,
                         test.elapsed / test.iterations * new_iters *
                             cfg.benchmarkSamples(),
                         CATCH_MOVE( fun ),
                         std::chrono::duration_cast<FDuration>(
                             cfg.benchmarkWarmupTime() ),
                         Detail::warmup_iterations };
            }

            std::vector<FDuration> run_plan_default( ExecutionPlan const& plan,
                                                     const IConfig& cfg,
                                                     Environment env ) {
                return plan.run<default_clock>( cfg, env );
            }

        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch
