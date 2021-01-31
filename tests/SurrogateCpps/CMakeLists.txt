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
  surr_catch_all.cpp
  surr_catch_approx.cpp
  surr_catch_assertion_info.cpp
  surr_catch_assertion_result.cpp
  surr_catch_config.cpp
  surr_catch_message.cpp
  #surr_catch_reporter_registrars.cpp #This does not build
  surr_catch_section_info.cpp
  surr_catch_session.cpp
  surr_catch_tag_alias.cpp
  surr_catch_tag_alias_autoregistrar.cpp
  surr_catch_template_test_macros.cpp
  surr_catch_test_case_info.cpp
  surr_catch_test_macros.cpp
  surr_catch_test_spec.cpp
  surr_catch_timer.cpp
  surr_catch_tostring.cpp
  surr_catch_totals.cpp
  surr_catch_translate_exception.cpp
  surr_catch_version.cpp
  surr_catch_version_macros.cpp
  surr_catch_generator_exception.cpp
  surr_catch_generators.cpp
  surr_catch_generators_adapters.cpp
  surr_catch_generators_all.cpp
  surr_catch_generators_random.cpp
  surr_catch_generators_range.cpp
  surr_catch_interfaces_all.cpp
  surr_catch_interfaces_capture.cpp
  surr_catch_interfaces_config.cpp
  surr_catch_interfaces_enum_values_registry.cpp
  surr_catch_interfaces_exception.cpp
  surr_catch_interfaces_generatortracker.cpp
  surr_catch_interfaces_registry_hub.cpp
  surr_catch_interfaces_reporter.cpp
  #surr_catch_interfaces_reporter_factory.cpp # This does not build
  surr_catch_interfaces_reporter_registry.cpp
  surr_catch_interfaces_runner.cpp
  surr_catch_interfaces_tag_alias_registry.cpp
  surr_catch_interfaces_testcase.cpp
  surr_catch_reporter_automake.cpp
  surr_catch_reporter_compact.cpp
  surr_catch_reporter_console.cpp
  surr_catch_reporter_cumulative_base.cpp
  surr_catch_reporter_event_listener.cpp
  surr_catch_reporter_helpers.cpp
  surr_catch_reporter_junit.cpp
  surr_catch_reporter_listening.cpp
  surr_catch_reporter_sonarqube.cpp
  surr_catch_reporter_streaming_base.cpp
  surr_catch_reporter_tap.cpp
  surr_catch_reporter_teamcity.cpp
  surr_catch_reporter_xml.cpp
  surr_catch_reporters_all.cpp
  )
add_executable(SurrogateTUsTest ${SURROGATE_TEST_SOURCES})
target_link_libraries(SurrogateTUsTest PRIVATE Catch2WithMain)

add_test(NAME SurrogateTUsTest COMMAND $<TARGET_FILE:SurrogateTUsTest>)
