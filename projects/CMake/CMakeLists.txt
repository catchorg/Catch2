cmake_minimum_required(VERSION 2.8)

project(Catch)

# define some folders
get_filename_component(CATCH_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PATH)
get_filename_component(CATCH_DIR "${CATCH_DIR}" PATH)

set(SELF_TEST_DIR ${CATCH_DIR}/projects/SelfTest)
set(INCLUDE_DIR ${CATCH_DIR}/include)

if(USE_CPP11)
  ## We can't turn this on by default, since it breaks on travis
  message(STATUS "Enabling C++11")
  set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif()

# define the sources of the self test
set(SOURCES
    ${SELF_TEST_DIR}/ApproxTests.cpp
    ${SELF_TEST_DIR}/BDDTests.cpp
    ${SELF_TEST_DIR}/ClassTests.cpp
    ${SELF_TEST_DIR}/ConditionTests.cpp
    ${SELF_TEST_DIR}/ExceptionTests.cpp
    ${SELF_TEST_DIR}/GeneratorTests.cpp
    ${SELF_TEST_DIR}/MessageTests.cpp
    ${SELF_TEST_DIR}/MiscTests.cpp
    ${SELF_TEST_DIR}/PartTrackerTests.cpp
    ${SELF_TEST_DIR}/TestMain.cpp
    ${SELF_TEST_DIR}/TrickyTests.cpp
    ${SELF_TEST_DIR}/VariadicMacrosTests.cpp
    ${SELF_TEST_DIR}/EnumToString.cpp
    ${SELF_TEST_DIR}/ToStringPair.cpp
    ${SELF_TEST_DIR}/ToStringVector.cpp
    ${SELF_TEST_DIR}/ToStringWhich.cpp
    ${SELF_TEST_DIR}/ToStringTuple.cpp

    # Catch include files, for in-IDE editing
    ${INCLUDE_DIR}/catch.hpp
    ${INCLUDE_DIR}/catch_session.hpp
    ${INCLUDE_DIR}/catch_with_main.hpp
    ${INCLUDE_DIR}/external/clara.h
    ${INCLUDE_DIR}/internal/catch_approx.hpp
    ${INCLUDE_DIR}/internal/catch_assertionresult.h
    ${INCLUDE_DIR}/internal/catch_assertionresult.hpp
    ${INCLUDE_DIR}/internal/catch_capture.hpp
    ${INCLUDE_DIR}/internal/catch_clara.h
    ${INCLUDE_DIR}/internal/catch_commandline.hpp
    ${INCLUDE_DIR}/internal/catch_common.h
    ${INCLUDE_DIR}/internal/catch_common.hpp
    ${INCLUDE_DIR}/internal/catch_compiler_capabilities.h
    ${INCLUDE_DIR}/internal/catch_config.hpp
    ${INCLUDE_DIR}/internal/catch_console_colour.hpp
    ${INCLUDE_DIR}/internal/catch_console_colour_impl.hpp
    ${INCLUDE_DIR}/internal/catch_context.h
    ${INCLUDE_DIR}/internal/catch_context_impl.hpp
    ${INCLUDE_DIR}/internal/catch_debugger.h
    ${INCLUDE_DIR}/internal/catch_debugger.hpp
    ${INCLUDE_DIR}/internal/catch_default_main.hpp
    ${INCLUDE_DIR}/internal/catch_evaluate.hpp
    ${INCLUDE_DIR}/internal/catch_exception_translator_registry.hpp
    ${INCLUDE_DIR}/internal/catch_expression_lhs.hpp
    ${INCLUDE_DIR}/internal/catch_fatal_condition.hpp
    ${INCLUDE_DIR}/internal/catch_generators.hpp
    ${INCLUDE_DIR}/internal/catch_generators_impl.hpp
    ${INCLUDE_DIR}/internal/catch_impl.hpp
    ${INCLUDE_DIR}/internal/catch_interfaces_capture.h
    ${INCLUDE_DIR}/internal/catch_interfaces_config.h
    ${INCLUDE_DIR}/internal/catch_interfaces_exception.h
    ${INCLUDE_DIR}/internal/catch_interfaces_generators.h
    ${INCLUDE_DIR}/internal/catch_interfaces_registry_hub.h
    ${INCLUDE_DIR}/internal/catch_interfaces_reporter.h
    ${INCLUDE_DIR}/internal/catch_interfaces_runner.h
    ${INCLUDE_DIR}/internal/catch_interfaces_tag_alias_registry.h
    ${INCLUDE_DIR}/internal/catch_interfaces_testcase.h
    ${INCLUDE_DIR}/internal/catch_legacy_reporter_adapter.h
    ${INCLUDE_DIR}/internal/catch_legacy_reporter_adapter.hpp
    ${INCLUDE_DIR}/internal/catch_list.hpp
    ${INCLUDE_DIR}/internal/catch_matchers.hpp
    ${INCLUDE_DIR}/internal/catch_message.h
    ${INCLUDE_DIR}/internal/catch_message.hpp
    ${INCLUDE_DIR}/internal/catch_notimplemented_exception.h
    ${INCLUDE_DIR}/internal/catch_notimplemented_exception.hpp
    ${INCLUDE_DIR}/internal/catch_objc.hpp
    ${INCLUDE_DIR}/internal/catch_objc_arc.hpp
    ${INCLUDE_DIR}/internal/catch_option.hpp
    ${INCLUDE_DIR}/internal/catch_platform.h
    ${INCLUDE_DIR}/internal/catch_ptr.hpp
    ${INCLUDE_DIR}/internal/catch_reenable_warnings.h
    ${INCLUDE_DIR}/internal/catch_registry_hub.hpp
    ${INCLUDE_DIR}/internal/catch_reporter_registrars.hpp
    ${INCLUDE_DIR}/internal/catch_reporter_registry.hpp
    ${INCLUDE_DIR}/internal/catch_result_builder.h
    ${INCLUDE_DIR}/internal/catch_result_builder.hpp
    ${INCLUDE_DIR}/internal/catch_result_type.h
    ${INCLUDE_DIR}/internal/catch_run_context.hpp
    ${INCLUDE_DIR}/internal/catch_section.h
    ${INCLUDE_DIR}/internal/catch_section.hpp
    ${INCLUDE_DIR}/internal/catch_section_info.h
    ${INCLUDE_DIR}/internal/catch_section_info.hpp
    ${INCLUDE_DIR}/internal/catch_stream.h
    ${INCLUDE_DIR}/internal/catch_stream.hpp
    ${INCLUDE_DIR}/internal/catch_streambuf.h
    ${INCLUDE_DIR}/internal/catch_suppress_warnings.h
    ${INCLUDE_DIR}/internal/catch_tag_alias.h
    ${INCLUDE_DIR}/internal/catch_tag_alias_registry.h
    ${INCLUDE_DIR}/internal/catch_tag_alias_registry.hpp
    ${INCLUDE_DIR}/internal/catch_test_case_info.h
    ${INCLUDE_DIR}/internal/catch_test_case_info.hpp
    ${INCLUDE_DIR}/internal/catch_test_case_registry_impl.hpp
    ${INCLUDE_DIR}/internal/catch_test_case_tracker.hpp
    ${INCLUDE_DIR}/internal/catch_test_registry.hpp
    ${INCLUDE_DIR}/internal/catch_test_spec.hpp
    ${INCLUDE_DIR}/internal/catch_test_spec_parser.hpp
    ${INCLUDE_DIR}/internal/catch_text.h
    ${INCLUDE_DIR}/internal/catch_timer.h
    ${INCLUDE_DIR}/internal/catch_timer.hpp
    ${INCLUDE_DIR}/internal/catch_tostring.h
    ${INCLUDE_DIR}/internal/catch_tostring.hpp
    ${INCLUDE_DIR}/internal/catch_totals.hpp
    ${INCLUDE_DIR}/internal/catch_version.h
    ${INCLUDE_DIR}/internal/catch_version.hpp
    ${INCLUDE_DIR}/internal/catch_wildcard_pattern.hpp
    ${INCLUDE_DIR}/internal/catch_xmlwriter.hpp
    ${INCLUDE_DIR}/reporters/catch_reporter_bases.hpp
    ${INCLUDE_DIR}/reporters/catch_reporter_compact.hpp
    ${INCLUDE_DIR}/reporters/catch_reporter_console.hpp
    ${INCLUDE_DIR}/reporters/catch_reporter_junit.hpp
    ${INCLUDE_DIR}/reporters/catch_reporter_multi.hpp
    ${INCLUDE_DIR}/reporters/catch_reporter_teamcity.hpp
    ${INCLUDE_DIR}/reporters/catch_reporter_xml.hpp
)

# configure the executable
include_directories(${CATCH_DIR}/include)
add_executable(SelfTest ${SOURCES})

# configure unit tests via CTest
enable_testing()
add_test(NAME RunTests COMMAND SelfTest)

add_test(NAME ListTests COMMAND SelfTest --list-tests)
set_tests_properties(ListTests PROPERTIES PASS_REGULAR_EXPRESSION "[0-9]+ test cases")

add_test(NAME ListTags COMMAND SelfTest --list-tags)
set_tests_properties(ListTags PROPERTIES PASS_REGULAR_EXPRESSION "[0-9]+ tags")
