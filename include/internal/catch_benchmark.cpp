/*
 *  Created by Phil on 04/07/2017.
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_benchmark.h"
#include "catch_capture.hpp"
#include "catch_interfaces_reporter.h"
#include "catch_context.h"
#include <cmath>

namespace Catch {

    auto BenchmarkLooper::getResolution() -> uint64_t {
        return getEstimatedClockResolution() * getCurrentContext().getConfig()->benchmarkResolutionMultiple();
    }

    void BenchmarkLooper::reportStart() {
        getResultCapture().benchmarkStarting( { m_name, m_minSamples } );
    }

    void BenchmarkLooper::reportEnd() {
        uint64_t totalNs = 0;
        uint64_t totalIterations = 0;
        for (auto const& m : m_timeStamps) {
            totalNs += m.m_elapsedTime;
            totalIterations += m.m_iterations;
        }
        const auto meanNsPerIteration = static_cast<double>(totalNs) / totalIterations;
        double stddev = 0;
        for (auto const& m : m_timeStamps) {
            const auto iterNs = static_cast<double>(m.m_elapsedTime) / m.m_iterations;
            const auto diffFromMean = iterNs - meanNsPerIteration;
            stddev += diffFromMean * diffFromMean * m.m_iterations;
        }
        stddev /= totalIterations;
        stddev = std::sqrt(stddev);

        getResultCapture().benchmarkEnded({ { m_name, m_minSamples }, totalIterations, m_timeStamps.size(), totalNs, static_cast<uint64_t>(stddev) });
    }

} // end namespace Catch
