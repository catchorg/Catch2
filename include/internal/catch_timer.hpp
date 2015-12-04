/*
 *  Created by Phil on 05/08/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_timer.h"
#include "catch_platform.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-long-long"
#endif

#ifdef CATCH_PLATFORM_WINDOWS
#include <windows.h>
#else
#include <sys/time.h>
#endif

namespace Catch {

    namespace {
#ifdef CATCH_PLATFORM_WINDOWS
        uint64_t getCurrentTicks() {
            static uint64_t hz=0, hzo=0;
            if (!hz) {
                QueryPerformanceFrequency( reinterpret_cast<LARGE_INTEGER*>( &hz ) );
                QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER*>( &hzo ) );
            }
            uint64_t t;
            QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER*>( &t ) );
            return ((t-hzo)*1000000)/hz;
        }
#else
        uint64_t getCurrentTicks() {
            timeval t;
            gettimeofday(&t,CATCH_NULL);
            return static_cast<uint64_t>( t.tv_sec ) * 1000000ull + static_cast<uint64_t>( t.tv_usec );
        }
#endif
    }

    void Timer::start() {
        m_ticks = getCurrentTicks();
    }
    unsigned int Timer::getElapsedMicroseconds() const {
        return static_cast<unsigned int>(getCurrentTicks() - m_ticks);
    }
    unsigned int Timer::getElapsedMilliseconds() const {
        return static_cast<unsigned int>(getElapsedMicroseconds()/1000);
    }
    double Timer::getElapsedSeconds() const {
        return getElapsedMicroseconds()/1000000.0;
    }

} // namespace Catch

#ifdef __clang__
#pragma clang diagnostic pop
#endif
