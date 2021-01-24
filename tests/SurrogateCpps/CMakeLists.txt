include(CTest)

set(SURROGATE_TEST_SOURCES
  surr_catch_benchmark.cpp
  surr_catch_benchmark_all.cpp
  surr_catch_chronometer.cpp
  surr_catch_clock.cpp
  #surr_catch_constructor.cpp #This does not build
  surr_catch_environment.cpp
  surr_catch_estimate.cpp
  surr_catch_execution_plan.cpp
  surr_catch_optimizer.cpp
  surr_catch_outlier_classification.cpp
  surr_catch_sample_analysis.cpp
  #surr_catch_analyse.cpp #This does not build
  surr_catch_benchmark_function.cpp
  surr_catch_complete_invoke.cpp
  surr_catch_estimate_clock.cpp
  surr_catch_measure.cpp
  surr_catch_repeat.cpp
  surr_catch_run_for_at_least.cpp
  surr_catch_stats.cpp
  surr_catch_timing.cpp
  )

add_executable(SurrogateTUsTest ${SURROGATE_TEST_SOURCES})
target_link_libraries(SurrogateTUsTest PRIVATE Catch2WithMain)

add_test(NAME SurrogateTUsTest COMMAND $<TARGET_FILE:SurrogateTUsTest>)
