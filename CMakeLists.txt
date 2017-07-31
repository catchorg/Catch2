cmake_minimum_required(VERSION 3.0)

project(CatchSelfTest)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# define some folders
set(CATCH_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SELF_TEST_DIR ${CATCH_DIR}/projects/SelfTest)
set(BENCHMARK_DIR ${CATCH_DIR}/projects/Benchmark)
set(HEADER_DIR ${CATCH_DIR}/include)

if(USE_CPP11)
    ## We can't turn this on by default, since it breaks on travis
    message(STATUS "Enabling C++11")
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
elseif(USE_CPP14)
    message(STATUS "Enabling C++14")
    set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")
endif()

if(USE_WMAIN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:wmainCRTStartup")
endif()

#checks that the given hard-coded list contains all headers + sources in the given folder
function(CheckFileList LIST_VAR FOLDER)
  set(MESSAGE " should be added to the variable ${LIST_VAR}")
  set(MESSAGE "${MESSAGE} in ${CMAKE_CURRENT_LIST_FILE}\n")
  file(GLOB GLOBBED_LIST "${FOLDER}/*.cpp"
                         "${FOLDER}/*.hpp"
                         "${FOLDER}/*.h")
  list(REMOVE_ITEM GLOBBED_LIST ${${LIST_VAR}})
  foreach(EXTRA_ITEM ${GLOBBED_LIST})
    string(REPLACE "${CATCH_DIR}/" "" RELATIVE_FILE_NAME "${EXTRA_ITEM}")
    message(AUTHOR_WARNING "The file \"${RELATIVE_FILE_NAME}\"${MESSAGE}")
  endforeach()
endfunction()

function(CheckFileListRec LIST_VAR FOLDER)
  set(MESSAGE " should be added to the variable ${LIST_VAR}")
  set(MESSAGE "${MESSAGE} in ${CMAKE_CURRENT_LIST_FILE}\n")
  file(GLOB_RECURSE GLOBBED_LIST "${FOLDER}/*.cpp"
                                 "${FOLDER}/*.hpp"
                                 "${FOLDER}/*.h")
  list(REMOVE_ITEM GLOBBED_LIST ${${LIST_VAR}})
  foreach(EXTRA_ITEM ${GLOBBED_LIST})
    string(REPLACE "${CATCH_DIR}/" "" RELATIVE_FILE_NAME "${EXTRA_ITEM}")
    message(AUTHOR_WARNING "The file \"${RELATIVE_FILE_NAME}\"${MESSAGE}")
  endforeach()
endfunction()

# define the sources of the self test
# Please keep these ordered alphabetically
set(TEST_SOURCES
        ${SELF_TEST_DIR}/ApproxTests.cpp
        ${SELF_TEST_DIR}/BDDTests.cpp
        ${SELF_TEST_DIR}/ClassTests.cpp
        ${SELF_TEST_DIR}/CmdLineTests.cpp
        ${SELF_TEST_DIR}/CompilationTests.cpp
        ${SELF_TEST_DIR}/ConditionTests.cpp
        ${SELF_TEST_DIR}/DecompositionTests.cpp
        ${SELF_TEST_DIR}/EnumToString.cpp
        ${SELF_TEST_DIR}/ExceptionTests.cpp
        ${SELF_TEST_DIR}/GeneratorTests.cpp
        ${SELF_TEST_DIR}/MessageTests.cpp
        ${SELF_TEST_DIR}/MiscTests.cpp
        ${SELF_TEST_DIR}/PartTrackerTests.cpp
        ${SELF_TEST_DIR}/TagAliasTests.cpp
        ${SELF_TEST_DIR}/TestMain.cpp
        ${SELF_TEST_DIR}/ToStringGeneralTests.cpp
        ${SELF_TEST_DIR}/ToStringPair.cpp
        ${SELF_TEST_DIR}/ToStringTuple.cpp
        ${SELF_TEST_DIR}/ToStringVector.cpp
        ${SELF_TEST_DIR}/ToStringWhich.cpp
        ${SELF_TEST_DIR}/TrickyTests.cpp
        ${SELF_TEST_DIR}/VariadicMacrosTests.cpp
        ${SELF_TEST_DIR}/MatchersTests.cpp
        )
CheckFileList(TEST_SOURCES ${SELF_TEST_DIR})

# A set of impl files that just #include a single header
# Please keep these ordered alphabetically
set(IMPL_SOURCES
        ${SELF_TEST_DIR}/SurrogateCpps/catch_common.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_console_colour.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_debugger.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_interfaces_capture.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_interfaces_config.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_interfaces_exception.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_interfaces_generators.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_interfaces_registry_hub.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_interfaces_reporter.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_interfaces_runner.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_interfaces_testcase.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_message.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_option.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_ptr.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_stream.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_streambuf.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_test_spec.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_xmlwriter.cpp
        ${SELF_TEST_DIR}/SurrogateCpps/catch_test_case_tracker.cpp
        )
CheckFileList(IMPL_SOURCES ${SELF_TEST_DIR}/SurrogateCpps)


# Please keep these ordered alphabetically
set(TOP_LEVEL_HEADERS
        ${HEADER_DIR}/catch.hpp
        ${HEADER_DIR}/catch_session.hpp
        ${HEADER_DIR}/catch_with_main.hpp
        )
CheckFileList(TOP_LEVEL_HEADERS ${HEADER_DIR})

# Please keep these ordered alphabetically
set(EXTERNAL_HEADERS
        ${HEADER_DIR}/external/clara.h
        ${HEADER_DIR}/external/tbc_text_format.h
        )
CheckFileList(EXTERNAL_HEADERS ${HEADER_DIR}/external)


# Please keep these ordered alphabetically
set(INTERNAL_HEADERS
        ${HEADER_DIR}/internal/catch_approx.hpp
        ${HEADER_DIR}/internal/catch_assertionresult.h
        ${HEADER_DIR}/internal/catch_assertionresult.hpp
        ${HEADER_DIR}/internal/catch_capture.hpp
        ${HEADER_DIR}/internal/catch_clara.h
        ${HEADER_DIR}/internal/catch_commandline.hpp
        ${HEADER_DIR}/internal/catch_common.h
        ${HEADER_DIR}/internal/catch_common.hpp
        ${HEADER_DIR}/internal/catch_compiler_capabilities.h
        ${HEADER_DIR}/internal/catch_config.hpp
        ${HEADER_DIR}/internal/catch_console_colour.hpp
        ${HEADER_DIR}/internal/catch_console_colour_impl.hpp
        ${HEADER_DIR}/internal/catch_context.h
        ${HEADER_DIR}/internal/catch_context_impl.hpp
        ${HEADER_DIR}/internal/catch_debugger.h
        ${HEADER_DIR}/internal/catch_debugger.hpp
        ${HEADER_DIR}/internal/catch_default_main.hpp
        ${HEADER_DIR}/internal/catch_errno_guard.hpp
        ${HEADER_DIR}/internal/catch_evaluate.hpp
        ${HEADER_DIR}/internal/catch_exception_translator_registry.hpp
        ${HEADER_DIR}/internal/catch_expression_lhs.hpp
        ${HEADER_DIR}/internal/catch_fatal_condition.hpp
        ${HEADER_DIR}/internal/catch_generators.hpp
        ${HEADER_DIR}/internal/catch_generators_impl.hpp
        ${HEADER_DIR}/internal/catch_impl.hpp
        ${HEADER_DIR}/internal/catch_interfaces_capture.h
        ${HEADER_DIR}/internal/catch_interfaces_config.h
        ${HEADER_DIR}/internal/catch_interfaces_exception.h
        ${HEADER_DIR}/internal/catch_interfaces_generators.h
        ${HEADER_DIR}/internal/catch_interfaces_registry_hub.h
        ${HEADER_DIR}/internal/catch_interfaces_reporter.h
        ${HEADER_DIR}/internal/catch_interfaces_runner.h
        ${HEADER_DIR}/internal/catch_interfaces_tag_alias_registry.h
        ${HEADER_DIR}/internal/catch_interfaces_testcase.h
        ${HEADER_DIR}/internal/catch_legacy_reporter_adapter.h
        ${HEADER_DIR}/internal/catch_legacy_reporter_adapter.hpp
        ${HEADER_DIR}/internal/catch_list.hpp
        ${HEADER_DIR}/internal/catch_matchers.hpp
        ${HEADER_DIR}/internal/catch_matchers_string.h
        ${HEADER_DIR}/internal/catch_matchers_string.hpp
        ${HEADER_DIR}/internal/catch_matchers_vector.h
        ${HEADER_DIR}/internal/catch_message.h
        ${HEADER_DIR}/internal/catch_message.hpp
        ${HEADER_DIR}/internal/catch_notimplemented_exception.h
        ${HEADER_DIR}/internal/catch_notimplemented_exception.hpp
        ${HEADER_DIR}/internal/catch_objc.hpp
        ${HEADER_DIR}/internal/catch_objc_arc.hpp
        ${HEADER_DIR}/internal/catch_option.hpp
        ${HEADER_DIR}/internal/catch_platform.h
        ${HEADER_DIR}/internal/catch_ptr.hpp
        ${HEADER_DIR}/internal/catch_reenable_warnings.h
        ${HEADER_DIR}/internal/catch_registry_hub.hpp
        ${HEADER_DIR}/internal/catch_reporter_registrars.hpp
        ${HEADER_DIR}/internal/catch_reporter_registry.hpp
        ${HEADER_DIR}/internal/catch_result_builder.h
        ${HEADER_DIR}/internal/catch_result_builder.hpp
        ${HEADER_DIR}/internal/catch_result_type.h
        ${HEADER_DIR}/internal/catch_run_context.hpp
        ${HEADER_DIR}/internal/catch_section.h
        ${HEADER_DIR}/internal/catch_section.hpp
        ${HEADER_DIR}/internal/catch_section_info.h
        ${HEADER_DIR}/internal/catch_section_info.hpp
        ${HEADER_DIR}/internal/catch_stream.h
        ${HEADER_DIR}/internal/catch_stream.hpp
        ${HEADER_DIR}/internal/catch_streambuf.h
        ${HEADER_DIR}/internal/catch_suppress_warnings.h
        ${HEADER_DIR}/internal/catch_tag_alias.h
        ${HEADER_DIR}/internal/catch_tag_alias_registry.h
        ${HEADER_DIR}/internal/catch_tag_alias_registry.hpp
        ${HEADER_DIR}/internal/catch_test_case_info.h
        ${HEADER_DIR}/internal/catch_test_case_info.hpp
        ${HEADER_DIR}/internal/catch_test_case_registry_impl.hpp
        ${HEADER_DIR}/internal/catch_test_case_tracker.hpp
        ${HEADER_DIR}/internal/catch_test_registry.hpp
        ${HEADER_DIR}/internal/catch_test_spec.hpp
        ${HEADER_DIR}/internal/catch_test_spec_parser.hpp
        ${HEADER_DIR}/internal/catch_text.h
        ${HEADER_DIR}/internal/catch_timer.h
        ${HEADER_DIR}/internal/catch_timer.hpp
        ${HEADER_DIR}/internal/catch_tostring.h
        ${HEADER_DIR}/internal/catch_tostring.hpp
        ${HEADER_DIR}/internal/catch_totals.hpp
        ${HEADER_DIR}/internal/catch_type_traits.hpp
        ${HEADER_DIR}/internal/catch_version.h
        ${HEADER_DIR}/internal/catch_version.hpp
        ${HEADER_DIR}/internal/catch_wildcard_pattern.hpp
        ${HEADER_DIR}/internal/catch_windows_h_proxy.h
        ${HEADER_DIR}/internal/catch_xmlwriter.hpp
        )
CheckFileList(INTERNAL_HEADERS ${HEADER_DIR}/internal)

# Please keep these ordered alphabetically
set(REPORTER_HEADERS
        ${HEADER_DIR}/reporters/catch_reporter_automake.hpp
        ${HEADER_DIR}/reporters/catch_reporter_bases.hpp
        ${HEADER_DIR}/reporters/catch_reporter_compact.hpp
        ${HEADER_DIR}/reporters/catch_reporter_console.hpp
        ${HEADER_DIR}/reporters/catch_reporter_junit.hpp
        ${HEADER_DIR}/reporters/catch_reporter_multi.hpp
        ${HEADER_DIR}/reporters/catch_reporter_tap.hpp
        ${HEADER_DIR}/reporters/catch_reporter_teamcity.hpp
        ${HEADER_DIR}/reporters/catch_reporter_xml.hpp
        )
CheckFileList(REPORTER_HEADERS ${HEADER_DIR}/reporters)

# Specify the headers, too, so CLion recognises them as project files
set(HEADERS
        ${TOP_LEVEL_HEADERS}
        ${EXTERNAL_HEADERS}
        ${INTERNAL_HEADERS}
        ${REPORTER_HEADERS}
        )


set(BENCH_SOURCES
        ${BENCHMARK_DIR}/BenchMain.cpp
        ${BENCHMARK_DIR}/StringificationBench.cpp
        )
CheckFileList(BENCH_SOURCES ${BENCHMARK_DIR})

# Provide some groupings for IDEs
SOURCE_GROUP("Tests" FILES ${TEST_SOURCES})
SOURCE_GROUP("Surrogates" FILES ${IMPL_SOURCES})
SOURCE_GROUP("Benchmarks" FILES ${BENCH_SOURCES})

# configure the executable
include_directories(${HEADER_DIR})

# Projects consuming Catch via ExternalProject_Add might want to use install step
# without building all of our selftests.
if (NOT NO_SELFTEST)
    add_executable(SelfTest ${TEST_SOURCES} ${IMPL_SOURCES} ${HEADERS})
    add_executable(Benchmark ${BENCH_SOURCES} ${HEADERS})

    # Add desired warnings
    if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
        target_compile_options( SelfTest PRIVATE -Wall -Wextra )
        target_compile_options( Benchmark PRIVATE -Wall -Wextra )
    endif()
    if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
        target_compile_options( SelfTest PRIVATE /W4 /w44265 /WX )
        target_compile_options( Benchmark PRIVATE /W4 )
    endif()


    # configure unit tests via CTest
    enable_testing()
    add_test(NAME RunTests COMMAND SelfTest)

    add_test(NAME ListTests COMMAND SelfTest --list-tests)
    set_tests_properties(ListTests PROPERTIES PASS_REGULAR_EXPRESSION "[0-9]+ test cases")

    add_test(NAME ListTags COMMAND SelfTest --list-tags)
    set_tests_properties(ListTags PROPERTIES PASS_REGULAR_EXPRESSION "[0-9]+ tags")

endif() # !NO_SELFTEST


install(DIRECTORY "single_include/" DESTINATION "include/catch")
