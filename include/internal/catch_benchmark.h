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

        std::string m_name;
        std::size_t m_count = 0;
        std::size_t m_iterationsToRun = 1;
        uint64_t m_resolution;
        Timer m_timer;

        static auto getResolution() -> uint64_t;
    public:
        // Keep most of this inline as it's on the code path that is being timed
        BenchmarkLooper( StringRef name )
        :   m_name( name ),
            m_resolution( getResolution() )
        {
            reportStart();
            m_timer.start();
        }

        explicit operator bool() {
            if( m_count < m_iterationsToRun )
                return true;
            return needsMoreIterations();
        }

        void increment() {
            ++m_count;
        }

        void reportStart();
        auto needsMoreIterations() -> bool;
    };

    class BenchmarkDeviationLooper {

        std::string m_name;
        std::size_t m_count = 0;
        std::size_t m_timeStampsToSample;
        Timer m_timer;
        std::vector<uint64_t> m_timeStamps;

        static auto getResolution() -> uint64_t;
    public:
        BenchmarkDeviationLooper( StringRef name, size_t timeStampsToSample )
        :   m_name( name ),
            m_timeStampsToSample( timeStampsToSample ),
            m_timeStamps( timeStampsToSample )
        {
            reportStart();
            m_timer.start();
        }

        explicit operator bool() {
            if( m_count < m_timeStampsToSample )
            {
                m_timeStamps [ m_count ] = m_timer.getElapsedNanoseconds();
                return true;
            }
            else
            {
                reportEnd();
                return false;
            }
        }

        void increment() {
            ++m_count;
        }

        void reportStart();
        void reportEnd();
        auto needsMoreIterations() -> bool;
    };

} // end namespace Catch

#define BENCHMARK( name ) \
    for( Catch::BenchmarkLooper looper( name ); looper; looper.increment() )
#define BENCHMARK_DEVIATION( name, timeStampsToSample ) \
    for( Catch::BenchmarkDeviationLooper looper( name, timeStampsToSample ); looper; looper.increment() )

#endif // TWOBLUECUBES_CATCH_BENCHMARK_H_INCLUDED
