
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// Adapted from donated nonius code.

#ifndef CATCH_SAMPLE_ANALYSIS_HPP_INCLUDED
#define CATCH_SAMPLE_ANALYSIS_HPP_INCLUDED

#include <catch2/benchmark/catch_clock.hpp>
#include <catch2/benchmark/catch_estimate.hpp>
#include <catch2/benchmark/catch_outlier_classification.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>

#include <algorithm>
#include <vector>
#include <iterator>

namespace Catch {
    namespace Benchmark {
        template <typename Duration>
        struct SampleAnalysis {
            std::vector<Duration> samples;
            Estimate<Duration> mean;
            Estimate<Duration> standard_deviation;
            OutlierClassification outliers;
            double outlier_variance;

            template <typename Duration2>
            operator SampleAnalysis<Duration2>() const {
                std::vector<Duration2> samples2;
                samples2.reserve(samples.size());
                std::transform(samples.begin(), samples.end(), std::back_inserter(samples2), [](Duration d) { return Duration2(d); });
                return {
                    CATCH_MOVE(samples2),
                    mean,
                    standard_deviation,
                    outliers,
                    outlier_variance,
                };
            }
        };
    } // namespace Benchmark
} // namespace Catch

#endif // CATCH_SAMPLE_ANALYSIS_HPP_INCLUDED
