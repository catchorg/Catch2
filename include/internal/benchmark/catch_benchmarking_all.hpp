/*
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */ 


// A proxy header that includes all of the benchmarking headers to allow
// concise include of the benchmarking features. You should prefer the
// individual includes in standard use.

#include "catch_benchmark.hpp"
#include "catch_chronometer.hpp"
#include "catch_clock.hpp"
#include "catch_constructor.hpp"
#include "catch_environment.hpp"
#include "catch_estimate.hpp"
#include "catch_execution_plan.hpp"
#include "catch_optimizer.hpp"
#include "catch_outlier_classification.hpp"
#include "catch_sample_analysis.hpp"
#include "detail/catch_analyse.hpp"
#include "detail/catch_benchmark_function.hpp"
#include "detail/catch_complete_invoke.hpp"
#include "detail/catch_estimate_clock.hpp"
#include "detail/catch_measure.hpp"
#include "detail/catch_repeat.hpp"
#include "detail/catch_run_for_at_least.hpp"
#include "detail/catch_stats.hpp"
#include "detail/catch_timing.hpp"
