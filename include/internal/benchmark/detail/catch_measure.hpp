/*
 *  Created by Joachim on 16/04/2019.
 *  Adapted from donated nonius code.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// Measure

#ifndef TWOBLUECUBES_CATCH_DETAIL_MEASURE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_DETAIL_MEASURE_HPP_INCLUDED

#include "../catch_clock.hpp"
#include "catch_complete_invoke.hpp"
#include "catch_timing.hpp"

#include <utility>

namespace Catch {
    namespace Benchmark {
        namespace Detail {
            template <typename Clock, typename Fun, typename... Args>
            TimingOf<Clock, Fun(Args...)> measure(Fun&& fun, Args&&... args) {
                auto start = Clock::now();
                auto&& r = Detail::complete_invoke(fun, std::forward<Args>(args)...);
                auto end = Clock::now();
                auto delta = end - start;
                return { delta, std::forward<decltype(r)>(r), 1 };
            }
        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_DETAIL_MEASURE_HPP_INCLUDED
