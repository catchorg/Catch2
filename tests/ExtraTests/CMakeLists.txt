#
# Build extra tests.
#

cmake_minimum_required( VERSION 3.5 )

project( Catch2ExtraTests LANGUAGES CXX )

message( STATUS "Extra tests included" )

# The MinDuration reporting tests do not need separate compilation, but
# they have non-trivial execution time, so they are categorized as
# extra tests, so that they are run less.
add_test(NAME MinDuration::SimpleThreshold COMMAND $<TARGET_FILE:SelfTest> --min-duration 0.950 [min_duration_test])
set_tests_properties(
    MinDuration::SimpleThreshold
  PROPERTIES
    PASS_REGULAR_EXPRESSION "s: sleep_for_1000ms"
    FAIL_REGULAR_EXPRESSION "sleep_for_100ms"
    RUN_SERIAL ON  # The test is timing sensitive, so we want to run it
    # serially to avoid false positives on oversubscribed machines
)

# -d yes overrides the threshold, so we should see the faster test even
# with a ridiculous high min duration threshold
add_test(NAME MinDuration::DurationOverrideYes COMMAND $<TARGET_FILE:SelfTest> --min-duration 1.0 -d yes [min_duration_test])
set_tests_properties(
    MinDuration::DurationOverrideYes
  PROPERTIES
    PASS_REGULAR_EXPRESSION "s: sleep_for_100ms"
)

# -d no overrides the threshold, so we should never see any tests even
# with ridiculously low min duration threshold
add_test(NAME MinDuration::DurationOverrideNo COMMAND $<TARGET_FILE:SelfTest> --min-duration 0.0001 -d no [min_duration_test])
set_tests_properties(
    MinDuration::DurationOverrideNo
  PROPERTIES
    FAIL_REGULAR_EXPRESSION "sleep_for_250ms"
)


# ------------ end of duration reporting tests

# define folders used:
set( TESTS_DIR   ${CATCH_DIR}/tests/ExtraTests )

add_executable(PrefixedMacros ${TESTS_DIR}/X01-PrefixedMacros.cpp)
target_compile_definitions( PrefixedMacros PRIVATE CATCH_CONFIG_PREFIX_ALL CATCH_CONFIG_RUNTIME_STATIC_REQUIRE )
# Macro configuration does not touch the compiled parts, so we can link
# it against the main library
target_link_libraries( PrefixedMacros Catch2WithMain )

add_test(NAME CATCH_CONFIG_PREFIX_ALL COMMAND PrefixedMacros -s)
set_tests_properties(
    CATCH_CONFIG_PREFIX_ALL
  PROPERTIES
    PASS_REGULAR_EXPRESSION "CATCH_"
    FAIL_REGULAR_EXPRESSION
    # The spaces are important -> They disambiguate between CATCH_REQUIRE
    # and REQUIRE without prefix.
    " REQUIRE; REQUIRE_FALSE; REQUIRE_THROWS; REQUIRE_THROWS_AS; REQUIRE_THROWS_WITH; REQUIRE_THROWS_MATCHES; REQUIRE_NOTHROW; CHECK; CHECK_FALSE; CHECKED_IF; CHECKED_ELSE; CHECK_NOFAIL; CHECK_THROWS; CHECK_THROWS_AS; CHECK_THROWS_WITH; CHECK_THROWS_MATCHES; CHECK_NOTHROW; REQUIRE_THAT; CHECK_THAT"
)


add_executable(DisabledMacros ${TESTS_DIR}/X02-DisabledMacros.cpp)
target_compile_definitions( DisabledMacros PRIVATE CATCH_CONFIG_DISABLE )
# Macro configuration does not touch the compiled parts, so we can link
# it against the main library
target_link_libraries( DisabledMacros Catch2WithMain )

add_test(NAME CATCH_CONFIG_DISABLE-1 COMMAND DisabledMacros -s)
set_tests_properties(
    CATCH_CONFIG_DISABLE-1
  PROPERTIES
    PASS_REGULAR_EXPRESSION "No tests ran"
    FAIL_REGULAR_EXPRESSION "This should not happen"
)
add_test(NAME CATCH_CONFIG_DISABLE-2 COMMAND DisabledMacros --list-tests)
set_tests_properties(
    CATCH_CONFIG_DISABLE-2
  PROPERTIES
    PASS_REGULAR_EXPRESSION "0 test cases"
)

add_executable( DisabledExceptions-DefaultHandler ${TESTS_DIR}/X03-DisabledExceptions-DefaultHandler.cpp )
add_executable( DisabledExceptions-CustomHandler ${TESTS_DIR}/X04-DisabledExceptions-CustomHandler.cpp )

foreach(target DisabledExceptions-DefaultHandler DisabledExceptions-CustomHandler)
  target_compile_options( ${target}
    PUBLIC
      $<$<CXX_COMPILER_ID:MSVC>:/EHs-c-;/D_HAS_EXCEPTIONS=0>
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:AppleClang>>:-fno-exceptions>
  )
  target_link_libraries(${target} Catch2_buildall_interface)
endforeach()
target_compile_definitions( DisabledExceptions-CustomHandler PUBLIC CATCH_CONFIG_DISABLE_EXCEPTIONS_CUSTOM_HANDLER )


add_test(NAME CATCH_CONFIG_DISABLE_EXCEPTIONS-1 COMMAND DisabledExceptions-DefaultHandler "Tests that run")
set_tests_properties(
    CATCH_CONFIG_DISABLE_EXCEPTIONS-1
  PROPERTIES
    PASS_REGULAR_EXPRESSION "assertions: 4 \| 2 passed \| 2 failed"
    FAIL_REGULAR_EXPRESSION "abort;terminate;fatal"
)


# The default handler on Windows leads to the just-in-time debugger firing,
# which makes this test unsuitable for CI and headless runs, as it opens
# up an interactive dialog.
if (NOT WIN32)
  add_test(NAME CATCH_CONFIG_DISABLE_EXCEPTIONS-2 COMMAND DisabledExceptions-DefaultHandler "Tests that abort")
  set_tests_properties(
      CATCH_CONFIG_DISABLE_EXCEPTIONS-2
    PROPERTIES
      PASS_REGULAR_EXPRESSION "Catch will terminate"
  )
endif(NOT WIN32)


add_test(NAME CATCH_CONFIG_DISABLE_EXCEPTIONS-3 COMMAND DisabledExceptions-CustomHandler "Tests that run")
set_tests_properties(
    CATCH_CONFIG_DISABLE_EXCEPTIONS-3
  PROPERTIES
    PASS_REGULAR_EXPRESSION "assertions: 4 \| 2 passed \| 2 failed"
    FAIL_REGULAR_EXPRESSION "====== CUSTOM HANDLER ======"
)

add_test(NAME CATCH_CONFIG_DISABLE_EXCEPTIONS-4 COMMAND DisabledExceptions-CustomHandler "Tests that abort")
set_tests_properties(
    CATCH_CONFIG_DISABLE_EXCEPTIONS-4
  PROPERTIES
    PASS_REGULAR_EXPRESSION "====== CUSTOM HANDLER ======"
)



add_executable(FallbackStringifier ${TESTS_DIR}/X10-FallbackStringifier.cpp)
target_compile_definitions( FallbackStringifier PRIVATE CATCH_CONFIG_FALLBACK_STRINGIFIER=fallbackStringifier )
target_link_libraries( FallbackStringifier Catch2WithMain )

add_test(NAME FallbackStringifier COMMAND FallbackStringifier -r compact -s)
set_tests_properties(
    FallbackStringifier
  PROPERTIES
    PASS_REGULAR_EXPRESSION "foo{} for: { !!! }"
)


add_executable(DisableStringification ${TESTS_DIR}/X11-DisableStringification.cpp)
target_compile_definitions( DisableStringification PRIVATE CATCH_CONFIG_DISABLE_STRINGIFICATION )
target_link_libraries(DisableStringification Catch2WithMain)
add_test(NAME CATCH_CONFIG_DISABLE_STRINGIFICATION COMMAND DisableStringification -r compact -s)
set_tests_properties(
    CATCH_CONFIG_DISABLE_STRINGIFICATION
  PROPERTIES
    PASS_REGULAR_EXPRESSION "Disabled by CATCH_CONFIG_DISABLE_STRINGIFICATION"
    FAIL_REGULAR_EXPRESSION  "Hidden{} == Hidden{}"
)


# This test touches windows.h, so it should only be compiled under msvc
if (MSVC)
    # This test fails if it does not compile and succeeds otherwise
    add_executable(WindowsHeader ${TESTS_DIR}/X90-WindowsHeaderInclusion.cpp)
    set_property( TARGET WindowsHeader PROPERTY CXX_STANDARD 14 )
    set_property( TARGET WindowsHeader PROPERTY CXX_STANDARD_REQUIRED ON )
    set_property( TARGET WindowsHeader PROPERTY CXX_EXTENSIONS OFF )
    target_link_libraries( WindowsHeader Catch2WithMain )
    add_test(NAME WindowsHeader COMMAND WindowsHeader -r compact)
    list(APPEND CATCH_WARNING_TARGETS ${EXTRA_TEST_BINARIES} WindowsHeader)
endif()

#add_executable(DebugBreakMacros ${TESTS_DIR}/X12-CustomDebugBreakMacro.cpp)
#target_link_libraries(DebugBreakMacros Catch2)
#add_test(NAME DebugBreakMacros COMMAND DebugBreakMacros --break)
#set_tests_properties(
#    DebugBreakMacros
#  PROPERTIES
#    PASS_REGULAR_EXPRESSION "Pretty please, break into debugger"
#)

set( EXTRA_TEST_BINARIES
    PrefixedMacros
    DisabledMacros
    DisabledExceptions-DefaultHandler
    DisabledExceptions-CustomHandler
    FallbackStringifier
    DisableStringification
#    DebugBreakMacros
)

# Shared config
foreach( test ${EXTRA_TEST_BINARIES} )
    set_property( TARGET ${test} PROPERTY CXX_STANDARD 14 )
    set_property( TARGET ${test} PROPERTY CXX_STANDARD_REQUIRED ON )
    set_property( TARGET ${test} PROPERTY CXX_EXTENSIONS OFF )
endforeach()

# Notice that we are modifying EXTRA_TEST_BINARIES destructively, do not
# use it after this point!
list(FILTER EXTRA_TEST_BINARIES EXCLUDE REGEX "DisabledExceptions.*")
list(APPEND CATCH_WARNING_TARGETS ${EXTRA_TEST_BINARIES})
set(CATCH_WARNING_TARGETS ${CATCH_WARNING_TARGETS} PARENT_SCOPE)


# This sets up a one-off executable that compiles against the amalgamated
# files, and then runs it for a super simple check that the amalgamated
# files are usable.
add_executable(AmalgamatedTestCompilation
  ${TESTS_DIR}/X91-AmalgamatedCatch.cpp
  ${CATCH_DIR}/extras/catch_amalgamated.hpp
  ${CATCH_DIR}/extras/catch_amalgamated.cpp
)
target_include_directories(AmalgamatedTestCompilation PRIVATE ${CATCH_DIR}/extras)
set_property( TARGET AmalgamatedTestCompilation PROPERTY CXX_STANDARD 14 )
set_property( TARGET AmalgamatedTestCompilation PROPERTY CXX_STANDARD_REQUIRED ON )
set_property( TARGET AmalgamatedTestCompilation PROPERTY CXX_EXTENSIONS OFF )

add_test(NAME AmalgamatedFileTest COMMAND AmalgamatedTestCompilation)
set_tests_properties(
    AmalgamatedFileTest
  PROPERTIES
    PASS_REGULAR_EXPRESSION "All tests passed \\(14 assertions in 3 test cases\\)"
)
