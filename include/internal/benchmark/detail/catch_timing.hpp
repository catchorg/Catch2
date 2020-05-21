/*
 *  Created by Joachim on 16/04/2019.
 *  Adapted from donated nonius code.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// Timing

#ifndef TWOBLUECUBES_CATCH_DETAIL_TIMING_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_DETAIL_TIMING_HPP_INCLUDED

#include "../catch_clock.hpp"
#include "catch_complete_invoke.hpp"

#include <tuple>
#include <type_traits>

namespace Catch {
    namespace Benchmark {
        template <typename Duration, typename Result>
        struct Timing {
            Duration elapsed;
            Result result;
            int iterations;
        };
        template <typename Clock, typename Func, typename... Args>
        using TimingOf = Timing<ClockDuration<Clock>, Detail::CompleteType_t<FunctionReturnType<Func, Args...>>>;
    } // namespace Benchmark
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_DETAIL_TIMING_HPP_INCLUDED
