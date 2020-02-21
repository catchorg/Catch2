include(MiscFunctions)

# Please keep these ordered alphabetically
set(BENCHMARK_HEADERS
    ${SOURCES_DIR}/benchmark/catch_benchmark.hpp
    ${SOURCES_DIR}/benchmark/catch_chronometer.hpp
    ${SOURCES_DIR}/benchmark/catch_clock.hpp
    ${SOURCES_DIR}/benchmark/catch_constructor.hpp
    ${SOURCES_DIR}/benchmark/catch_environment.hpp
    ${SOURCES_DIR}/benchmark/catch_estimate.hpp
    ${SOURCES_DIR}/benchmark/catch_execution_plan.hpp
    ${SOURCES_DIR}/benchmark/catch_optimizer.hpp
    ${SOURCES_DIR}/benchmark/catch_outlier_classification.hpp
    ${SOURCES_DIR}/benchmark/catch_sample_analysis.hpp
    ${SOURCES_DIR}/benchmark/detail/catch_analyse.hpp
    ${SOURCES_DIR}/benchmark/detail/catch_benchmark_function.hpp
    ${SOURCES_DIR}/benchmark/detail/catch_complete_invoke.hpp
    ${SOURCES_DIR}/benchmark/detail/catch_estimate_clock.hpp
    ${SOURCES_DIR}/benchmark/detail/catch_measure.hpp
    ${SOURCES_DIR}/benchmark/detail/catch_repeat.hpp
    ${SOURCES_DIR}/benchmark/detail/catch_run_for_at_least.hpp
    ${SOURCES_DIR}/benchmark/detail/catch_stats.hpp
    ${SOURCES_DIR}/benchmark/detail/catch_timing.hpp
)
set(BENCHMARK_SOURCES
    ${SOURCES_DIR}/benchmark/catch_chronometer.cpp
    ${SOURCES_DIR}/benchmark/detail/catch_benchmark_function.cpp
    ${SOURCES_DIR}/benchmark/detail/catch_complete_invoke.cpp
    ${SOURCES_DIR}/benchmark/detail/catch_run_for_at_least.cpp
    ${SOURCES_DIR}/benchmark/detail/catch_stats.cpp
)

SOURCE_GROUP("benchmark" FILES ${BENCHMARK_HEADERS} ${BENCHMARK_SOURCES})

set(INTERNAL_HEADERS
    ${SOURCES_DIR}/catch.hpp
    ${SOURCES_DIR}/catch_approx.h
    ${SOURCES_DIR}/catch_assertionhandler.h
    ${SOURCES_DIR}/catch_assertioninfo.h
    ${SOURCES_DIR}/catch_assertionresult.h
    ${SOURCES_DIR}/catch_capture.hpp
    ${SOURCES_DIR}/internal/catch_capture_matchers.h
    ${SOURCES_DIR}/catch_clara.h
    ${SOURCES_DIR}/catch_commandline.h
    ${SOURCES_DIR}/catch_common.h
    ${SOURCES_DIR}/catch_compiler_capabilities.h
    ${SOURCES_DIR}/catch_config.hpp
    ${SOURCES_DIR}/catch_console_colour.h
    ${SOURCES_DIR}/catch_context.h
    ${SOURCES_DIR}/catch_debug_console.h
    ${SOURCES_DIR}/catch_debugger.h
    ${SOURCES_DIR}/catch_decomposer.h
    ${SOURCES_DIR}/catch_default_main.hpp
    ${SOURCES_DIR}/catch_enforce.h
    ${SOURCES_DIR}/catch_enum_values_registry.h
    ${SOURCES_DIR}/catch_errno_guard.h
    ${SOURCES_DIR}/catch_exception_translator_registry.h
    ${SOURCES_DIR}/catch_fatal_condition.h
    ${SOURCES_DIR}/catch_generator_exception.hpp
    ${SOURCES_DIR}/catch_generators.hpp
    ${SOURCES_DIR}/catch_generators_generic.hpp
    ${SOURCES_DIR}/catch_generators_specific.hpp
    ${SOURCES_DIR}/catch_impl.hpp
    ${SOURCES_DIR}/catch_interfaces_capture.h
    ${SOURCES_DIR}/catch_interfaces_config.h
    ${SOURCES_DIR}/catch_interfaces_enum_values_registry.h
    ${SOURCES_DIR}/catch_interfaces_exception.h
    ${SOURCES_DIR}/catch_interfaces_registry_hub.h
    ${SOURCES_DIR}/catch_interfaces_reporter.h
    ${SOURCES_DIR}/catch_interfaces_runner.h
    ${SOURCES_DIR}/catch_interfaces_tag_alias_registry.h
    ${SOURCES_DIR}/catch_interfaces_testcase.h
    ${SOURCES_DIR}/catch_leak_detector.h
    ${SOURCES_DIR}/catch_list.h
    ${SOURCES_DIR}/matchers/catch_matchers.h
    ${SOURCES_DIR}/matchers/catch_matchers_exception.hpp
    ${SOURCES_DIR}/matchers/catch_matchers_floating.h
    ${SOURCES_DIR}/matchers/catch_matchers_generic.hpp
    ${SOURCES_DIR}/matchers/catch_matchers_string.h
    ${SOURCES_DIR}/matchers/catch_matchers_templates.hpp
    ${SOURCES_DIR}/matchers/catch_matchers_vector.h
    ${SOURCES_DIR}/catch_message.h
    ${SOURCES_DIR}/catch_meta.hpp
    ${SOURCES_DIR}/catch_objc.hpp
    ${SOURCES_DIR}/catch_objc_arc.hpp
    ${SOURCES_DIR}/catch_option.hpp
    ${SOURCES_DIR}/catch_output_redirect.h
    ${SOURCES_DIR}/catch_platform.h
    ${SOURCES_DIR}/catch_polyfills.hpp
    ${SOURCES_DIR}/catch_preprocessor.hpp
    ${SOURCES_DIR}/catch_random_number_generator.h
    ${SOURCES_DIR}/catch_reenable_warnings.h
    ${SOURCES_DIR}/catch_reporter_registrars.hpp
    ${SOURCES_DIR}/catch_reporter_registry.h
    ${SOURCES_DIR}/catch_result_type.h
    ${SOURCES_DIR}/catch_run_context.h
    ${SOURCES_DIR}/catch_section.h
    ${SOURCES_DIR}/catch_section_info.h
    ${SOURCES_DIR}/catch_session.h
    ${SOURCES_DIR}/catch_singletons.hpp
    ${SOURCES_DIR}/catch_startup_exception_registry.h
    ${SOURCES_DIR}/catch_stream.h
    ${SOURCES_DIR}/catch_stringref.h
    ${SOURCES_DIR}/catch_string_manip.h
    ${SOURCES_DIR}/catch_suppress_warnings.h
    ${SOURCES_DIR}/catch_tag_alias.h
    ${SOURCES_DIR}/catch_tag_alias_autoregistrar.h
    ${SOURCES_DIR}/catch_tag_alias_registry.h
    ${SOURCES_DIR}/catch_test_case_info.h
    ${SOURCES_DIR}/catch_test_case_registry_impl.h
    ${SOURCES_DIR}/catch_test_case_tracker.h
    ${SOURCES_DIR}/catch_test_macros.hpp
    ${SOURCES_DIR}/catch_test_registry.h
    ${SOURCES_DIR}/catch_test_spec.h
    ${SOURCES_DIR}/catch_test_spec_parser.h
    ${SOURCES_DIR}/catch_text.h
    ${SOURCES_DIR}/catch_timer.h
    ${SOURCES_DIR}/catch_to_string.hpp
    ${SOURCES_DIR}/catch_tostring.h
    ${SOURCES_DIR}/catch_totals.h
    ${SOURCES_DIR}/catch_uncaught_exceptions.h
    ${SOURCES_DIR}/catch_user_interfaces.h
    ${SOURCES_DIR}/catch_version.h
    ${SOURCES_DIR}/catch_version_macros.hpp
    ${SOURCES_DIR}/catch_wildcard_pattern.h
    ${SOURCES_DIR}/catch_windows_h_proxy.h
    ${SOURCES_DIR}/catch_xmlwriter.h
)
set(IMPL_SOURCES
    ${SOURCES_DIR}/catch_approx.cpp
    ${SOURCES_DIR}/catch_assertionhandler.cpp
    ${SOURCES_DIR}/catch_assertionresult.cpp
    ${SOURCES_DIR}/internal/catch_capture_matchers.cpp
    ${SOURCES_DIR}/catch_commandline.cpp
    ${SOURCES_DIR}/catch_common.cpp
    ${SOURCES_DIR}/catch_config.cpp
    ${SOURCES_DIR}/catch_console_colour.cpp
    ${SOURCES_DIR}/catch_context.cpp
    ${SOURCES_DIR}/catch_debug_console.cpp
    ${SOURCES_DIR}/catch_debugger.cpp
    ${SOURCES_DIR}/catch_decomposer.cpp
    ${SOURCES_DIR}/catch_enforce.cpp
    ${SOURCES_DIR}/catch_enum_values_registry.cpp
    ${SOURCES_DIR}/catch_errno_guard.cpp
    ${SOURCES_DIR}/catch_exception_translator_registry.cpp
    ${SOURCES_DIR}/catch_fatal_condition.cpp
    ${SOURCES_DIR}/catch_generator_exception.cpp
    ${SOURCES_DIR}/catch_generators.cpp
    ${SOURCES_DIR}/catch_interfaces_capture.cpp
    ${SOURCES_DIR}/catch_interfaces_config.cpp
    ${SOURCES_DIR}/catch_interfaces_exception.cpp
    ${SOURCES_DIR}/catch_interfaces_generatortracker.h
    ${SOURCES_DIR}/catch_interfaces_registry_hub.cpp
    ${SOURCES_DIR}/catch_interfaces_runner.cpp
    ${SOURCES_DIR}/catch_interfaces_testcase.cpp
    ${SOURCES_DIR}/catch_list.cpp
    ${SOURCES_DIR}/catch_leak_detector.cpp
    ${SOURCES_DIR}/matchers/catch_matchers.cpp
    ${SOURCES_DIR}/matchers/catch_matchers_exception.cpp
    ${SOURCES_DIR}/matchers/catch_matchers_floating.cpp
    ${SOURCES_DIR}/matchers/catch_matchers_generic.cpp
    ${SOURCES_DIR}/matchers/catch_matchers_string.cpp
    ${SOURCES_DIR}/matchers/catch_matchers_templates.cpp
    ${SOURCES_DIR}/catch_message.cpp
    ${SOURCES_DIR}/catch_output_redirect.cpp
    ${SOURCES_DIR}/catch_registry_hub.cpp
    ${SOURCES_DIR}/catch_interfaces_reporter.cpp
    ${SOURCES_DIR}/catch_polyfills.cpp
    ${SOURCES_DIR}/catch_random_number_generator.cpp
    ${SOURCES_DIR}/catch_reporter_registry.cpp
    ${SOURCES_DIR}/catch_result_type.cpp
    ${SOURCES_DIR}/catch_run_context.cpp
    ${SOURCES_DIR}/catch_section.cpp
    ${SOURCES_DIR}/catch_session.cpp
    ${SOURCES_DIR}/catch_singletons.cpp
    ${SOURCES_DIR}/catch_startup_exception_registry.cpp
    ${SOURCES_DIR}/catch_stream.cpp
    ${SOURCES_DIR}/catch_stringref.cpp
    ${SOURCES_DIR}/catch_string_manip.cpp
    ${SOURCES_DIR}/catch_tag_alias.cpp
    ${SOURCES_DIR}/catch_tag_alias_autoregistrar.cpp
    ${SOURCES_DIR}/catch_tag_alias_registry.cpp
    ${SOURCES_DIR}/catch_test_case_info.cpp
    ${SOURCES_DIR}/catch_test_case_registry_impl.cpp
    ${SOURCES_DIR}/catch_test_case_tracker.cpp
    ${SOURCES_DIR}/catch_test_registry.cpp
    ${SOURCES_DIR}/catch_test_spec.cpp
    ${SOURCES_DIR}/catch_test_spec_parser.cpp
    ${SOURCES_DIR}/catch_timer.cpp
    ${SOURCES_DIR}/catch_tostring.cpp
    ${SOURCES_DIR}/catch_totals.cpp
    ${SOURCES_DIR}/catch_uncaught_exceptions.cpp
    ${SOURCES_DIR}/catch_version.cpp
    ${SOURCES_DIR}/catch_wildcard_pattern.cpp
    ${SOURCES_DIR}/catch_xmlwriter.cpp
)
set(INTERNAL_FILES ${IMPL_SOURCES} ${INTERNAL_HEADERS})
CheckFileList(INTERNAL_FILES ${SOURCES_DIR}/internal)

# Please keep these ordered alphabetically
set(REPORTER_HEADERS
    ${SOURCES_DIR}/reporters/catch_reporter_automake.hpp
    ${SOURCES_DIR}/reporters/catch_reporter_bases.hpp
    ${SOURCES_DIR}/reporters/catch_reporter_compact.h
    ${SOURCES_DIR}/reporters/catch_reporter_console.h
    ${SOURCES_DIR}/reporters/catch_reporter_junit.h
    ${SOURCES_DIR}/reporters/catch_reporter_listening.h
    ${SOURCES_DIR}/reporters/catch_reporter_sonarqube.hpp
    ${SOURCES_DIR}/reporters/catch_reporter_tap.hpp
    ${SOURCES_DIR}/reporters/catch_reporter_teamcity.hpp
    ${SOURCES_DIR}/reporters/catch_reporter_xml.h
)
set(REPORTER_SOURCES
    ${SOURCES_DIR}/reporters/catch_reporter_automake.cpp
    ${SOURCES_DIR}/reporters/catch_reporter_bases.cpp
    ${SOURCES_DIR}/reporters/catch_reporter_compact.cpp
    ${SOURCES_DIR}/reporters/catch_reporter_console.cpp
    ${SOURCES_DIR}/reporters/catch_reporter_junit.cpp
    ${SOURCES_DIR}/reporters/catch_reporter_listening.cpp
    ${SOURCES_DIR}/reporters/catch_reporter_sonarqube.cpp
    ${SOURCES_DIR}/reporters/catch_reporter_tap.cpp
    ${SOURCES_DIR}/reporters/catch_reporter_teamcity.cpp
    ${SOURCES_DIR}/reporters/catch_reporter_xml.cpp
)
set(REPORTER_FILES ${REPORTER_HEADERS} ${REPORTER_SOURCES})
CheckFileList(REPORTER_FILES ${SOURCES_DIR}/reporters)

# Fixme: STATIC because for dynamic, we would need to handle visibility
# and I don't want to do the annotations right now
add_library(Catch2 STATIC
  ${REPORTER_FILES}
  ${INTERNAL_FILES}
  ${BENCHMARK_HEADERS}
  ${BENCHMARK_SOURCES}
)

if (ANDROID)
    target_link_libraries(Catch2 INTERFACE log)
endif()

# depend on bunch of C++11 and C++14 features to have C++14 enabled by default
target_compile_features(Catch2
  PUBLIC
    cxx_alignas
    cxx_alignof
    cxx_attributes
    cxx_auto_type
    cxx_constexpr
    cxx_defaulted_functions
    cxx_deleted_functions
    cxx_final
    cxx_lambdas
    cxx_noexcept
    cxx_override
    cxx_range_for
    cxx_rvalue_references
    cxx_static_assert
    cxx_strong_enums
    cxx_trailing_return_types
    cxx_unicode_literals
    cxx_user_literals
    cxx_variable_templates
    cxx_variadic_macros
)

target_include_directories(Catch2
  PUBLIC
    $<BUILD_INTERFACE:${SOURCES_DIR}/..>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)


add_library(Catch2Main STATIC
    ${SOURCES_DIR}/catch_main.cpp
)
add_library(Catch2::Catch2Main ALIAS Catch2Main)
target_link_libraries(Catch2Main PUBLIC Catch2)


if (NOT_SUBPROJECT)
    # create and install an export set for catch target as Catch2::Catch
    install(
      TARGETS
        Catch2
        Catch2Main
      EXPORT
        Catch2Targets
      DESTINATION
        ${CMAKE_INSTALL_LIBDIR}
    )


    install(
      EXPORT
        Catch2Targets
      NAMESPACE
        Catch2::
      DESTINATION
        ${CATCH_CMAKE_CONFIG_DESTINATION}
    )
    # Install the headers
    install(DIRECTORY ${SOURCES_DIR} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h*")
endif()

# Some tests require a full recompilation of Catch2 lib with different
# compilation flags. They can link against this target to recompile all
# the sources into the binary.
if (CATCH_BUILD_EXAMPLES OR CATCH_BUILD_EXTRA_TESTS)
    add_library(Catch2_buildall_interface INTERFACE)
    target_sources(Catch2_buildall_interface INTERFACE
      ${REPORTER_FILES}
      ${INTERNAL_FILES}
      ${BENCHMARK_HEADERS}
      ${BENCHMARK_SOURCES}
    )
    target_include_directories(Catch2_buildall_interface
      INTERFACE
        $<BUILD_INTERFACE:${SOURCES_DIR}/..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_compile_features(Catch2_buildall_interface
      INTERFACE
        cxx_alignas
        cxx_alignof
        cxx_attributes
        cxx_auto_type
        cxx_constexpr
        cxx_defaulted_functions
        cxx_deleted_functions
        cxx_final
        cxx_lambdas
        cxx_noexcept
        cxx_override
        cxx_range_for
        cxx_rvalue_references
        cxx_static_assert
        cxx_strong_enums
        cxx_trailing_return_types
        cxx_unicode_literals
        cxx_user_literals
        cxx_variable_templates
        cxx_variadic_macros
    )
endif()

list(APPEND CATCH_WARNING_TARGETS Catch2 Catch2Main)
set(CATCH_WARNING_TARGETS ${CATCH_WARNING_TARGETS} PARENT_SCOPE)
