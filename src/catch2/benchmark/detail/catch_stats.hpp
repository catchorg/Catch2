
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// Adapted from donated nonius code.

#ifndef CATCH_STATS_HPP_INCLUDED
#define CATCH_STATS_HPP_INCLUDED

#include <catch2/benchmark/catch_estimate.hpp>
#include <catch2/benchmark/catch_outlier_classification.hpp>

#include <algorithm>
#include <vector>
#include <cmath>

namespace Catch {
    namespace Benchmark {
        namespace Detail {
            using sample = std::vector<double>;

            // Used when we know we want == comparison of two doubles
            // to centralize warning suppression
            bool directCompare( double lhs, double rhs );

            double weighted_average_quantile(int k, int q, std::vector<double>::iterator first, std::vector<double>::iterator last);

            OutlierClassification
            classify_outliers( std::vector<double>::const_iterator first,
                               std::vector<double>::const_iterator last );

            double mean( std::vector<double>::const_iterator first,
                         std::vector<double>::const_iterator last );

            template <typename Estimator>
            sample jackknife(Estimator&& estimator,
                             std::vector<double>::iterator first,
                             std::vector<double>::iterator last) {
                auto n = static_cast<size_t>(last - first);
                auto second = first;
                ++second;
                sample results;
                results.reserve(n);

                for (auto it = first; it != last; ++it) {
                    std::iter_swap(it, first);
                    results.push_back(estimator(second, last));
                }

                return results;
            }

            inline double normal_cdf(double x) {
                return std::erfc(-x / std::sqrt(2.0)) / 2.0;
            }

            double erfc_inv(double x);

            double normal_quantile(double p);

            template <typename Estimator>
            Estimate<double> bootstrap( double confidence_level,
                                        std::vector<double>::iterator first,
                                        std::vector<double>::iterator last,
                                        sample const& resample,
                                        Estimator&& estimator ) {
                auto n_samples = last - first;

                double point = estimator(first, last);
                // Degenerate case with a single sample
                if (n_samples == 1) return { point, point, point, confidence_level };

                sample jack = jackknife(estimator, first, last);
                double jack_mean = mean(jack.begin(), jack.end());
                double sum_squares = 0, sum_cubes = 0;
                for (double x : jack) {
                    auto difference = jack_mean - x;
                    auto square = difference * difference;
                    auto cube = square * difference;
                    sum_squares += square; sum_cubes += cube;
                }

                double accel = sum_cubes / (6 * std::pow(sum_squares, 1.5));
                long n = static_cast<long>(resample.size());
                double prob_n = std::count_if(resample.begin(), resample.end(), [point](double x) { return x < point; }) / static_cast<double>(n);
                // degenerate case with uniform samples
                if ( directCompare( prob_n, 0. ) ) {
                    return { point, point, point, confidence_level };
                }

                double bias = normal_quantile(prob_n);
                double z1 = normal_quantile((1. - confidence_level) / 2.);

                auto cumn = [n]( double x ) -> long {
                    return std::lround( normal_cdf( x ) * static_cast<double>(n) );
                };
                auto a = [bias, accel](double b) { return bias + b / (1. - accel * b); };
                double b1 = bias + z1;
                double b2 = bias - z1;
                double a1 = a(b1);
                double a2 = a(b2);
                auto lo = static_cast<size_t>((std::max)(cumn(a1), 0l));
                auto hi = static_cast<size_t>((std::min)(cumn(a2), n - 1));

                return { point, resample[lo], resample[hi], confidence_level };
            }

            struct bootstrap_analysis {
                Estimate<double> mean;
                Estimate<double> standard_deviation;
                double outlier_variance;
            };

            bootstrap_analysis analyse_samples(double confidence_level,
                                               unsigned int n_resamples,
                                               std::vector<double>::iterator first,
                                               std::vector<double>::iterator last);
        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch

#endif // CATCH_STATS_HPP_INCLUDED
