
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// Adapted from donated nonius code.

#ifndef CATCH_ANALYSE_HPP_INCLUDED
#define CATCH_ANALYSE_HPP_INCLUDED

#include <catch2/benchmark/catch_clock.hpp>
#include <catch2/benchmark/catch_environment.hpp>
#include <catch2/benchmark/catch_sample_analysis.hpp>
#include <catch2/benchmark/detail/catch_stats.hpp>
#include <catch2/interfaces/catch_interfaces_config.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>

#include <algorithm>
#include <iterator>
#include <vector>

namespace Catch {
    namespace Benchmark {
        namespace Detail {
            template <typename Duration, typename Iterator>
            SampleAnalysis<Duration> analyse(const IConfig &cfg, Environment<Duration>, Iterator first, Iterator last) {
                if (!cfg.benchmarkNoAnalysis()) {
                    std::vector<double> samples;
                    samples.reserve(static_cast<size_t>(last - first));
                    std::transform(first, last, std::back_inserter(samples), [](Duration d) { return d.count(); });

                    auto analysis = Catch::Benchmark::Detail::analyse_samples(cfg.benchmarkConfidenceInterval(), cfg.benchmarkResamples(), samples.begin(), samples.end());
                    auto outliers = Catch::Benchmark::Detail::classify_outliers(samples.begin(), samples.end());

                    auto wrap_estimate = [](Estimate<double> e) {
                        return Estimate<Duration> {
                            Duration(e.point),
                                Duration(e.lower_bound),
                                Duration(e.upper_bound),
                                e.confidence_interval,
                        };
                    };
                    std::vector<Duration> samples2;
                    samples2.reserve(samples.size());
                    std::transform(samples.begin(), samples.end(), std::back_inserter(samples2), [](double d) { return Duration(d); });
                    return {
                        CATCH_MOVE(samples2),
                        wrap_estimate(analysis.mean),
                        wrap_estimate(analysis.standard_deviation),
                        outliers,
                        analysis.outlier_variance,
                    };
                } else {
                    std::vector<Duration> samples;
                    samples.reserve(static_cast<size_t>(last - first));

                    Duration mean = Duration(0);
                    int i = 0;
                    for (auto it = first; it < last; ++it, ++i) {
                        samples.push_back(Duration(*it));
                        mean += Duration(*it);
                    }
                    mean /= i;

                    return {
                        CATCH_MOVE(samples),
                        Estimate<Duration>{mean, mean, mean, 0.0},
                        Estimate<Duration>{Duration(0), Duration(0), Duration(0), 0.0},
                        OutlierClassification{},
                        0.0
                    };
                }
            }
        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch

#endif // CATCH_ANALYSE_HPP_INCLUDED
