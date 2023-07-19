
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_BENCHMARK_STATS_HPP_INCLUDED
#define CATCH_BENCHMARK_STATS_HPP_INCLUDED

#include <catch2/internal/catch_move_and_forward.hpp>
#include <catch2/benchmark/catch_estimate.hpp>
#include <catch2/benchmark/catch_outlier_classification.hpp>
// The fwd decl & default specialization needs to be seen by VS2017 before
// BenchmarkStats itself, or VS2017 will report compilation error.
#include <catch2/benchmark/detail/catch_benchmark_stats_fwd.hpp>

#include <string>
#include <vector>

namespace Catch {

    struct BenchmarkInfo {
        std::string name;
        double estimatedDuration;
        int iterations;
        unsigned int samples;
        unsigned int resamples;
        double clockResolution;
        double clockCost;
    };

    template <class Duration>
    struct BenchmarkStats {
        BenchmarkInfo info;

        std::vector<Duration> samples;
        Benchmark::Estimate<Duration> mean;
        Benchmark::Estimate<Duration> standardDeviation;
        Benchmark::OutlierClassification outliers;
        double outlierVariance;

        template <typename Duration2>
        operator BenchmarkStats<Duration2>() const {
            std::vector<Duration2> samples2;
            samples2.reserve(samples.size());
            for (auto const& sample : samples) {
                samples2.push_back(Duration2(sample));
            }
            return {
                info,
                CATCH_MOVE(samples2),
                mean,
                standardDeviation,
                outliers,
                outlierVariance,
            };
        }
    };


} // end namespace Catch

#endif // CATCH_BENCHMARK_STATS_HPP_INCLUDED
