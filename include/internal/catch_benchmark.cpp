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

namespace Catch {

    auto BenchmarkLooper::getResolution() -> uint64_t {
        return getEstimatedClockResolution() * getCurrentContext().getConfig()->benchmarkResolutionMultiple();
    }

    void BenchmarkLooper::reportStart() {
        getResultCapture().benchmarkStarting( { m_name } );
    }
    auto BenchmarkLooper::needsMoreIterations() -> bool {
        auto elapsed = m_timer.getElapsedNanoseconds();

        // Exponentially increasing iterations until we're confident in our timer resolution
        if( elapsed < m_resolution ) {
            m_iterationsToRun *= 10;
            return true;
        }

        getResultCapture().benchmarkEnded( { { m_name }, m_count, elapsed, 0 } );
        return false;
    }

    void BenchmarkDeviationLooper::reportStart() {
        getResultCapture().benchmarkStarting( { m_name } );
    }
    void BenchmarkDeviationLooper::reportEnd() {
        auto elapsed = m_timer.getElapsedNanoseconds();
        if (m_timeStampsToSample < 3)
        {
            getResultCapture().benchmarkEnded( { { m_name }, m_count, elapsed, 0 } );
            return;
        }

        // Convert timestamps to delta timestamps
        for (size_t i = 0; i + 1 < m_timeStamps.size(); ++i)
            m_timeStamps[i] = m_timeStamps[i + 1] - m_timeStamps[i];
        // Note N timestamps produce N-1 delta timestamps
        m_timeStamps.pop_back();

        double mean = (double)elapsed / (double)m_count;
        double sigma = 0.0;
        for (auto v: m_timeStamps)
            sigma += (v - mean) * (v - mean);
        sigma = sqrt(sigma / (double)(m_timeStamps.size() - 1));

        getResultCapture().benchmarkEnded( { { m_name }, m_count, elapsed, (uint64_t)sigma } );
    }

} // end namespace Catch
