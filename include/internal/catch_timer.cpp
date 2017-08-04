/*
 *  Created by Phil on 05/08/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_timer.h"

#include <chrono>

namespace Catch {

    auto getCurrentNanosecondsSinceEpoch() -> uint64_t {
        return std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() ).count();
    }

        void Timer::start() {
           m_nanoseconds = getCurrentNanosecondsSinceEpoch();
        }
        auto Timer::getElapsedNanoseconds() const -> unsigned int {
            return static_cast<unsigned int>(getCurrentNanosecondsSinceEpoch() - m_nanoseconds);
        }
        auto Timer::getElapsedMicroseconds() const -> unsigned int {
            return static_cast<unsigned int>(getElapsedNanoseconds()/1000);
        }
        auto Timer::getElapsedMilliseconds() const -> unsigned int {
            return static_cast<unsigned int>(getElapsedMicroseconds()/1000);
        }
        auto Timer::getElapsedSeconds() const -> double {
            return getElapsedMicroseconds()/1000000.0;
        }


} // namespace Catch
