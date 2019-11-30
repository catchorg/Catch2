/*
 *  Created by Joachim on 16/04/2019.
 *  Adapted from donated nonius code.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// Environment information

#ifndef TWOBLUECUBES_CATCH_ENVIRONMENT_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_ENVIRONMENT_HPP_INCLUDED

#include "catch_clock.hpp"
#include "catch_outlier_classification.hpp"

namespace Catch {
    namespace Benchmark {
        template <typename Duration>
        struct EnvironmentEstimate {
            Duration mean;
            OutlierClassification outliers;

            template <typename Duration2>
            operator EnvironmentEstimate<Duration2>() const {
                return { mean, outliers };
            }
        };
        template <typename Clock>
        struct Environment {
            using clock_type = Clock;
            EnvironmentEstimate<FloatDuration<Clock>> clock_resolution;
            EnvironmentEstimate<FloatDuration<Clock>> clock_cost;
        };
    } // namespace Benchmark
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_ENVIRONMENT_HPP_INCLUDED
