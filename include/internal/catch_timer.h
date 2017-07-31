/*
 *  Created by Phil on 05/08/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TIMER_H_INCLUDED
#define TWOBLUECUBES_CATCH_TIMER_H_INCLUDED

#include "catch_platform.h"

#ifdef _MSC_VER

namespace Catch {
    typedef unsigned long long UInt64;
}
#else
#include <stdint.h>
namespace Catch {
    typedef uint64_t UInt64;
}
#endif


namespace Catch {
    class Timer {
    public:
        Timer() : m_ticks( 0 ) {}
        void start();
        unsigned int getElapsedMicroseconds() const;
        unsigned int getElapsedMilliseconds() const;
        double getElapsedSeconds() const;

    private:
        UInt64 m_ticks;
    };

} // namespace Catch

#endif // TWOBLUECUBES_CATCH_TIMER_H_INCLUDED
