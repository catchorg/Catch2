# License: Boost 1.0
# By Paul Dreik 2020
set(INT ${CATCH_DIR}/include/internal/)
set(INC ${CATCH_DIR}/include)

add_library(fuzzlib
    ${INT}/catch_string_manip.cpp
    ${INT}/catch_approx.cpp
    ${INT}/catch_assertionhandler.cpp
    ${INT}/catch_assertionresult.cpp
    ${INT}/catch_capture_matchers.cpp
    ${INT}/catch_commandline.cpp
    ${INT}/catch_common.cpp
    ${INT}/catch_config.cpp
    ${INT}/catch_console_colour.cpp
    ${INT}/catch_context.cpp
    ${INT}/catch_debug_console.cpp
    ${INT}/catch_debugger.cpp
    ${INT}/catch_decomposer.cpp
    ${INT}/catch_enforce.cpp
    ${INT}/catch_enum_values_registry.cpp
    ${INT}/catch_errno_guard.cpp
    ${INT}/catch_exception_translator_registry.cpp
    ${INT}/catch_fatal_condition.cpp
    ${INT}/catch_generators.cpp
    ${INT}/catch_interfaces_capture.cpp
    ${INT}/catch_interfaces_config.cpp
    ${INT}/catch_interfaces_exception.cpp
    ${INT}/catch_interfaces_registry_hub.cpp
    ${INT}/catch_interfaces_reporter.cpp
    ${INT}/catch_interfaces_runner.cpp
    ${INT}/catch_interfaces_testcase.cpp
    ${INT}/catch_leak_detector.cpp
    ${INT}/catch_list.cpp
    ${INT}/catch_matchers.cpp
    ${INT}/catch_matchers_exception.cpp
    ${INT}/catch_matchers_floating.cpp
    ${INT}/catch_matchers_generic.cpp
    ${INT}/catch_matchers_string.cpp
    ${INT}/catch_message.cpp
    ${INT}/catch_output_redirect.cpp
    ${INT}/catch_polyfills.cpp
    ${INT}/catch_random_number_generator.cpp
    ${INT}/catch_registry_hub.cpp
    ${INT}/catch_reporter_registry.cpp
    ${INT}/catch_result_type.cpp
    ${INT}/catch_run_context.cpp
    ${INT}/catch_section.cpp
    ${INT}/catch_section_info.cpp
    ${INT}/catch_session.cpp
    ${INT}/catch_singletons.cpp
    ${INT}/catch_startup_exception_registry.cpp
    ${INT}/catch_stream.cpp
    ${INT}/catch_string_manip.cpp
    ${INT}/catch_stringref.cpp
    ${INT}/catch_tag_alias_autoregistrar.cpp
    ${INT}/catch_tag_alias.cpp
    ${INT}/catch_tag_alias_registry.cpp
    ${INT}/catch_test_case_info.cpp
    ${INT}/catch_test_case_registry_impl.cpp
    ${INT}/catch_test_case_tracker.cpp
    ${INT}/catch_test_registry.cpp
    ${INT}/catch_test_spec.cpp
    ${INT}/catch_test_spec_parser.cpp
    ${INT}/catch_timer.cpp
    ${INT}/catch_tostring.cpp
    ${INT}/catch_totals.cpp
    ${INT}/catch_uncaught_exceptions.cpp
    ${INT}/catch_version.cpp
    ${INT}/catch_wildcard_pattern.cpp
    ${INT}/catch_xmlwriter.cpp

    ${INC}/reporters/catch_reporter_bases.cpp
    ${INC}/reporters/catch_reporter_compact.cpp
    ${INC}/reporters/catch_reporter_console.cpp
    ${INC}/reporters/catch_reporter_junit.cpp
    ${INC}/reporters/catch_reporter_listening.cpp
    ${INC}/reporters/catch_reporter_xml.cpp
    )

target_include_directories(fuzzlib PUBLIC ${CATCH_DIR}/include)

add_executable(fuzz_TestSpecParser fuzz_TestSpecParser.cpp)
target_link_libraries(fuzz_TestSpecParser PRIVATE fuzzlib)

add_executable(fuzz_XmlWriter fuzz_XmlWriter.cpp)
target_link_libraries(fuzz_XmlWriter PRIVATE fuzzlib)


# This should be possible to set from the outside to be oss-fuzz compatible,
# fix later. For now, be libFuzzer only.
target_link_options(fuzz_TestSpecParser PRIVATE "-fsanitize=fuzzer")
target_link_options(fuzz_XmlWriter PRIVATE "-fsanitize=fuzzer")
