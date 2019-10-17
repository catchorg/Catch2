#
# Build extra tests.
#
# Requires CATCH_BUILD_EXTRA_TESTS to be defined 'true', see ../CMakeLists.txt.
#

cmake_minimum_required( VERSION 3.5 )

project( Catch2ExtraTests LANGUAGES CXX )

message( STATUS "Extra tests included" )

# define folders used:

set( TESTS_DIR   ${CATCH_DIR}/projects/ExtraTests )
set( SINGLE_INCLUDE_PATH ${CATCH_DIR}/single_include )


add_executable(PrefixedMacros ${TESTS_DIR}/X01-PrefixedMacros.cpp)
target_compile_definitions( PrefixedMacros PRIVATE CATCH_CONFIG_PREFIX_ALL )

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
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/EHs-c-;/D_HAS_EXCEPTIONS=0>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:AppleClang>>:-fno-exceptions>
#    $<$<CXX_COMPILER_ID:Clang>:-fno-exceptions>
#    $<$<CXX_COMPILER_ID:GNU>:-fno-exceptions>
)
endforeach()


add_test(NAME CATCH_CONFIG_DISABLE_EXCEPTIONS-1 COMMAND DisabledExceptions-DefaultHandler "Tests that run")
set_tests_properties(
    CATCH_CONFIG_DISABLE_EXCEPTIONS-1
  PROPERTIES
    PASS_REGULAR_EXPRESSION "assertions: 4 \| 2 passed \| 2 failed"
    FAIL_REGULAR_EXPRESSION "abort;terminate;fatal"
)

add_test(NAME CATCH_CONFIG_DISABLE_EXCEPTIONS-2 COMMAND DisabledExceptions-DefaultHandler "Tests that abort")
set_tests_properties(
    CATCH_CONFIG_DISABLE_EXCEPTIONS-2
  PROPERTIES
    PASS_REGULAR_EXPRESSION "Catch will terminate"
)

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

add_test(NAME FallbackStringifier COMMAND FallbackStringifier -r compact -s)
set_tests_properties(
    FallbackStringifier
  PROPERTIES
    PASS_REGULAR_EXPRESSION "foo{} for: { !!! }"
)


add_executable(DisableStringification ${TESTS_DIR}/X11-DisableStringification.cpp)
target_compile_definitions( DisableStringification PRIVATE CATCH_CONFIG_DISABLE_STRINGIFICATION )
add_test(NAME CATCH_CONFIG_DISABLE_STRINGIFICATION COMMAND DisableStringification -r compact -s)
set_tests_properties(
    CATCH_CONFIG_DISABLE_STRINGIFICATION
  PROPERTIES
    PASS_REGULAR_EXPRESSION "Disabled by CATCH_CONFIG_DISABLE_STRINGIFICATION"
    FAIL_REGULAR_EXPRESSION  "Hidden{} == Hidden{}"
)


add_executable(BenchmarkingMacros ${TESTS_DIR}/X20-BenchmarkingMacros.cpp)
target_compile_definitions( BenchmarkingMacros PRIVATE CATCH_CONFIG_ENABLE_BENCHMARKING )

add_test(NAME BenchmarkingMacros COMMAND BenchmarkingMacros -r console -s)
set_tests_properties(
    BenchmarkingMacros
  PROPERTIES
    PASS_REGULAR_EXPRESSION "benchmark name                                  samples       iterations    estimated"
)

# This test touches windows.h, so it should only be compiled under msvc
if (MSVC)
    # This test fails if it does not compile and succeeds otherwise
    add_executable(WindowsHeader ${TESTS_DIR}/X90-WindowsHeaderInclusion.cpp)
    set_property( TARGET WindowsHeader PROPERTY CXX_STANDARD 11 )
    set_property( TARGET WindowsHeader PROPERTY CXX_STANDARD_REQUIRED ON )
    set_property( TARGET WindowsHeader PROPERTY CXX_EXTENSIONS OFF )
    target_include_directories( WindowsHeader PRIVATE ${SINGLE_INCLUDE_PATH} )
    add_test(NAME WindowsHeader COMMAND WindowsHeader -r compact)
endif()

set( EXTRA_TEST_BINARIES
    PrefixedMacros
    DisabledMacros
    DisabledExceptions-DefaultHandler
    DisabledExceptions-CustomHandler
    FallbackStringifier
    DisableStringification
    BenchmarkingMacros
)

# Shared config
foreach( test ${EXTRA_TEST_BINARIES} )
    set_property( TARGET ${test} PROPERTY CXX_STANDARD 11 )
    set_property( TARGET ${test} PROPERTY CXX_STANDARD_REQUIRED ON )
    set_property( TARGET ${test} PROPERTY CXX_EXTENSIONS OFF )
    target_include_directories( ${test} PRIVATE ${SINGLE_INCLUDE_PATH} )
endforeach()


