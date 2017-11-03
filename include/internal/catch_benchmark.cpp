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

        getResultCapture().benchmarkEnded( { { m_name }, m_count, elapsed } );
        return false;
    }

} // end namespace Catch
