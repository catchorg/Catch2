/*
 *  Created by Phil on 04/07/2017.
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_BENCHMARK_H_INCLUDED
#define TWOBLUECUBES_CATCH_BENCHMARK_H_INCLUDED

#include "catch_stringref.h"
#include "catch_timer.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Catch {

    class BenchmarkLooper {

        struct MeasurementRecord {
            uint64_t m_elapsedTime;
            uint64_t m_iterations;
        };

        std::string m_name;
        std::size_t m_count = 0;
        std::size_t m_iterationsToRun = 1;
        std::size_t m_minSamples;
        uint64_t m_resolution;
        std::vector<MeasurementRecord> m_timeStamps;
        Timer m_timer;

        static auto getResolution() -> uint64_t;
    public:
        // Keep most of this inline as it's on the code path that is being timed
        BenchmarkLooper( StringRef name, size_t minSamples = 10 )
        :   m_name( name ),
            m_minSamples(minSamples),
            m_resolution( getResolution() )
        {
            reportStart();
            m_timeStamps.reserve(minSamples);
            m_timer.start();
        }

        explicit operator bool() {
            if (m_count < m_iterationsToRun) {
                return true;
            }
            auto elapsed = m_timer.getElapsedNanoseconds();
            // Exponentially increasing iterations until we're confident in our timer resolution
            if (elapsed < m_resolution) {
                m_iterationsToRun *= 10;
                return true;
            }
            // If this sample run is done, we need to save the results
            // and start again
            if (m_timeStamps.size() < m_minSamples) {
                m_timeStamps.push_back({ elapsed, m_count });
                // restart the outer iteration counter
                m_count = 0;
                m_timer.start();
                return true;
            }
            reportEnd();
            return false;
        }

        void increment() {
            ++m_count;
        }

        void reportStart();
        void reportEnd();
    };

} // end namespace Catch

#define BENCHMARK( ... ) \
    for( Catch::BenchmarkLooper looper( __VA_ARGS__ ); looper; looper.increment() )

#endif // TWOBLUECUBES_CATCH_BENCHMARK_H_INCLUDED
