cmake_minimum_required(VERSION 2.8)

project(Catch)

# define some folders
get_filename_component(CATCH_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PATH)
get_filename_component(CATCH_DIR "${CATCH_DIR}" PATH)
set(SELF_TEST_DIR ${CATCH_DIR}/projects/SelfTest)

# checks that the given hard-coded list contains all headers and sources in the given folder
function(CheckFileList HARD_CODED_LIST_VAR RELATIVE_FOLDER)
  set(MESSAGE " should be added to the variable ${HARD_CODED_LIST_VAR}")
  set(MESSAGE "${MESSAGE} in ${CMAKE_CURRENT_LIST_FILE}\n")
  file(GLOB GLOBBED_LIST "${CATCH_DIR}/${RELATIVE_FOLDER}/*.cpp"
                         "${CATCH_DIR}/${RELATIVE_FOLDER}/*.hpp"
                         "${CATCH_DIR}/${RELATIVE_FOLDER}/*.h")
  list(REMOVE_ITEM GLOBBED_LIST ${${HARD_CODED_LIST_VAR}})
  foreach(EXTRA_ITEM ${GLOBBED_LIST})
    string(REPLACE "${CATCH_DIR}/" "" RELATIVE_FILE_NAME "${EXTRA_ITEM}")
    message(AUTHOR_WARNING "The file \"${RELATIVE_FILE_NAME}\"${MESSAGE}")
  endforeach()
endfunction()

# define the sources of Catch
set(CATCH_HEADERS
    "${CATCH_DIR}/include/catch.hpp"
    "${CATCH_DIR}/include/catch_runner.hpp"
    "${CATCH_DIR}/include/catch_with_main.hpp"
)
source_group("include" FILES ${CATCH_HEADERS})
CheckFileList(CATCH_HEADERS "include")

set(CATCH_INTERNAL_HEADERS
    "${CATCH_DIR}/include/internal/catch_approx.hpp"
    "${CATCH_DIR}/include/internal/catch_assertionresult.h"
    "${CATCH_DIR}/include/internal/catch_assertionresult.hpp"
    "${CATCH_DIR}/include/internal/catch_capture.hpp"
    "${CATCH_DIR}/include/internal/catch_commandline.hpp"
    "${CATCH_DIR}/include/internal/catch_common.h"
    "${CATCH_DIR}/include/internal/catch_compiler_capabilities.h"
    "${CATCH_DIR}/include/internal/catch_config.hpp"
    "${CATCH_DIR}/include/internal/catch_console_colour.hpp"
    "${CATCH_DIR}/include/internal/catch_console_colour_impl.hpp"
    "${CATCH_DIR}/include/internal/catch_context.h"
    "${CATCH_DIR}/include/internal/catch_context_impl.hpp"
    "${CATCH_DIR}/include/internal/catch_debugger.hpp"
    "${CATCH_DIR}/include/internal/catch_default_main.hpp"
    "${CATCH_DIR}/include/internal/catch_evaluate.hpp"
    "${CATCH_DIR}/include/internal/catch_exception_translator_registry.hpp"
    "${CATCH_DIR}/include/internal/catch_expressionresult_builder.h"
    "${CATCH_DIR}/include/internal/catch_expressionresult_builder.hpp"
    "${CATCH_DIR}/include/internal/catch_expression_decomposer.hpp"
    "${CATCH_DIR}/include/internal/catch_expression_lhs.hpp"
    "${CATCH_DIR}/include/internal/catch_generators.hpp"
    "${CATCH_DIR}/include/internal/catch_generators_impl.hpp"
    "${CATCH_DIR}/include/internal/catch_impl.hpp"
    "${CATCH_DIR}/include/internal/catch_interfaces_capture.h"
    "${CATCH_DIR}/include/internal/catch_interfaces_config.h"
    "${CATCH_DIR}/include/internal/catch_interfaces_exception.h"
    "${CATCH_DIR}/include/internal/catch_interfaces_generators.h"
    "${CATCH_DIR}/include/internal/catch_interfaces_registry_hub.h"
    "${CATCH_DIR}/include/internal/catch_interfaces_reporter.h"
    "${CATCH_DIR}/include/internal/catch_interfaces_runner.h"
    "${CATCH_DIR}/include/internal/catch_interfaces_testcase.h"
    "${CATCH_DIR}/include/internal/catch_legacy_reporter_adapter.h"
    "${CATCH_DIR}/include/internal/catch_legacy_reporter_adapter.hpp"
    "${CATCH_DIR}/include/internal/catch_list.hpp"
    "${CATCH_DIR}/include/internal/catch_matchers.hpp"
    "${CATCH_DIR}/include/internal/catch_message.h"
    "${CATCH_DIR}/include/internal/catch_message.hpp"
    "${CATCH_DIR}/include/internal/catch_notimplemented_exception.h"
    "${CATCH_DIR}/include/internal/catch_notimplemented_exception.hpp"
    "${CATCH_DIR}/include/internal/catch_objc.hpp"
    "${CATCH_DIR}/include/internal/catch_objc_arc.hpp"
    "${CATCH_DIR}/include/internal/catch_option.hpp"
    "${CATCH_DIR}/include/internal/catch_platform.h"
    "${CATCH_DIR}/include/internal/catch_ptr.hpp"
    "${CATCH_DIR}/include/internal/catch_registry_hub.hpp"
    "${CATCH_DIR}/include/internal/catch_reporter_registrars.hpp"
    "${CATCH_DIR}/include/internal/catch_reporter_registry.hpp"
    "${CATCH_DIR}/include/internal/catch_result_type.h"
    "${CATCH_DIR}/include/internal/catch_runner_impl.hpp"
    "${CATCH_DIR}/include/internal/catch_section.hpp"
    "${CATCH_DIR}/include/internal/catch_section_info.hpp"
    "${CATCH_DIR}/include/internal/catch_sfinae.hpp"
    "${CATCH_DIR}/include/internal/catch_stream.hpp"
    "${CATCH_DIR}/include/internal/catch_streambuf.h"
    "${CATCH_DIR}/include/internal/catch_tags.hpp"
    "${CATCH_DIR}/include/internal/catch_test_case_info.h"
    "${CATCH_DIR}/include/internal/catch_test_case_info.hpp"
    "${CATCH_DIR}/include/internal/catch_test_case_registry_impl.hpp"
    "${CATCH_DIR}/include/internal/catch_test_case_tracker.hpp"
    "${CATCH_DIR}/include/internal/catch_test_registry.hpp"
    "${CATCH_DIR}/include/internal/catch_test_spec.h"
    "${CATCH_DIR}/include/internal/catch_text.h"
    "${CATCH_DIR}/include/internal/catch_text.hpp"
    "${CATCH_DIR}/include/internal/catch_timer.h"
    "${CATCH_DIR}/include/internal/catch_timer.hpp"
    "${CATCH_DIR}/include/internal/catch_tostring.hpp"
    "${CATCH_DIR}/include/internal/catch_totals.hpp"
    "${CATCH_DIR}/include/internal/catch_version.h"
    "${CATCH_DIR}/include/internal/catch_version.hpp"
    "${CATCH_DIR}/include/internal/catch_xmlwriter.hpp"
    "${CATCH_DIR}/include/internal/clara.h"
)
source_group("include\\internal" FILES ${CATCH_INTERNAL_HEADERS})
CheckFileList(CATCH_INTERNAL_HEADERS "include/internal")

set(CATCH_REPORTERS_HEADERS
    "${CATCH_DIR}/include/reporters/catch_reporter_console.hpp"
    "${CATCH_DIR}/include/reporters/catch_reporter_junit.hpp"
    "${CATCH_DIR}/include/reporters/catch_reporter_xml.hpp"
)
source_group("include\\reporters" FILES ${CATCH_REPORTERS_HEADERS})
CheckFileList(CATCH_REPORTERS_HEADERS "include/reporters")

set(CATCH_SINGLE_INCLUDE "${CATCH_DIR}/single_include/catch.hpp")
source_group("single_include" FILES ${CATCH_SINGLE_INCLUDE})
CheckFileList(CATCH_SINGLE_INCLUDE "single_include")

# define the sources of the self test
set(TEST_SOURCES
    "${SELF_TEST_DIR}/ApproxTests.cpp"
    "${SELF_TEST_DIR}/BDDTests.cpp"
    "${SELF_TEST_DIR}/ClassTests.cpp"
    "${SELF_TEST_DIR}/CmdLineTests.cpp"
    "${SELF_TEST_DIR}/ConditionTests.cpp"
    "${SELF_TEST_DIR}/ExceptionTests.cpp"
    "${SELF_TEST_DIR}/GeneratorTests.cpp"
    "${SELF_TEST_DIR}/MessageTests.cpp"
    "${SELF_TEST_DIR}/MiscTests.cpp"
    "${SELF_TEST_DIR}/SectionTrackerTests.cpp"
    "${SELF_TEST_DIR}/TestMain.cpp"
    "${SELF_TEST_DIR}/TrickyTests.cpp"
    "${SELF_TEST_DIR}/VariadicMacrosTests.cpp"
)
CheckFileList(TEST_SOURCES "projects/SelfTest")

# add custom targets comprising Catch header files
add_custom_target(Catch SOURCES
    ${CATCH_HEADERS}
    ${CATCH_INTERNAL_HEADERS}
    ${CATCH_REPORTERS_HEADERS}
    ${CATCH_SINGLE_INCLUDE}
)

# configure the test executable
include_directories(${CATCH_DIR}/include)
add_executable(SelfTest ${TEST_SOURCES})

# configure unit tests via CTest
enable_testing()
add_test(NAME RunTests COMMAND SelfTest)

add_test(NAME ListTests COMMAND SelfTest --list-tests)
set_tests_properties(ListTests PROPERTIES PASS_REGULAR_EXPRESSION "[0-9]+ test cases")

add_test(NAME ListTags COMMAND SelfTest --list-tags)
set_tests_properties(ListTags PROPERTIES PASS_REGULAR_EXPRESSION "[0-9]+ tags")
