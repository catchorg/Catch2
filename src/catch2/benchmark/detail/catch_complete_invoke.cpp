/*
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <catch2/benchmark/detail/catch_complete_invoke.hpp>

namespace Catch {
    namespace Benchmark {
        namespace Detail {
            CATCH_INTERNAL_START_WARNINGS_SUPPRESSION
            CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS
            const std::string benchmarkErrorMsg = "a benchmark failed to run successfully";
            CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION
        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch
