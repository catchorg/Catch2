/*
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// Run a function for a minimum amount of time
#include <catch2/benchmark/detail/catch_run_for_at_least.hpp>

#include <exception>
#include <catch2/catch_enforce.h>

namespace Catch {
    namespace Benchmark {
        namespace Detail {
            struct optimized_away_error : std::exception {
                const char* what() const noexcept override;
            };

            const char* optimized_away_error::what() const noexcept {
                return "could not measure benchmark, maybe it was optimized away";
            }

            void throw_optimized_away_error() {
                Catch::throw_exception(optimized_away_error{});
            }

        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch
