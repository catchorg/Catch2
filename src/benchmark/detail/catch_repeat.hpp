/*
 *  Created by Joachim on 16/04/2019.
 *  Adapted from donated nonius code.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// repeat algorithm

#ifndef TWOBLUECUBES_CATCH_DETAIL_REPEAT_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_DETAIL_REPEAT_HPP_INCLUDED

#include <type_traits>
#include <utility>

namespace Catch {
    namespace Benchmark {
        namespace Detail {
            template <typename Fun>
            struct repeater {
                void operator()(int k) const {
                    for (int i = 0; i < k; ++i) {
                        fun();
                    }
                }
                Fun fun;
            };
            template <typename Fun>
            repeater<typename std::decay<Fun>::type> repeat(Fun&& fun) {
                return { std::forward<Fun>(fun) };
            }
        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_DETAIL_REPEAT_HPP_INCLUDED
