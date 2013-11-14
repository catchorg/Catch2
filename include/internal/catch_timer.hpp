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
        INTERNAL_CATCH_INLINE uint64_t getCurrentTicks() {
            static uint64_t hz=0, hzo=0;
            if (!hz) {
                QueryPerformanceFrequency((LARGE_INTEGER*)&hz);
                QueryPerformanceCounter((LARGE_INTEGER*)&hzo);
            }
            uint64_t t;
            QueryPerformanceCounter((LARGE_INTEGER*)&t);
            return ((t-hzo)*1000000)/hz;
        }
#else
        INTERNAL_CATCH_INLINE uint64_t getCurrentTicks() {
            timeval t;
            gettimeofday(&t,NULL);
            return (uint64_t)t.tv_sec * 1000000ull + (uint64_t)t.tv_usec;
        }
#endif
    }

    INTERNAL_CATCH_INLINE void Timer::start() {
        m_ticks = getCurrentTicks();
    }
    INTERNAL_CATCH_INLINE unsigned int Timer::getElapsedNanoseconds() const {
        return (unsigned int)(getCurrentTicks() - m_ticks);
    }
    INTERNAL_CATCH_INLINE unsigned int Timer::getElapsedMilliseconds() const {
        return (unsigned int)((getCurrentTicks() - m_ticks)/1000);
    }
    INTERNAL_CATCH_INLINE double Timer::getElapsedSeconds() const {
        return (getCurrentTicks() - m_ticks)/1000000.0;
    }

} // namespace Catch

#ifdef __clang__
#pragma clang diagnostic pop
#endif
