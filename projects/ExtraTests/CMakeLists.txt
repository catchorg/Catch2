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


set( EXTRA_TEST_BINARIES
    PrefixedMacros
    DisabledMacros
    FallbackStringifier
    DisableStringification
)

# Shared config
foreach( test ${EXTRA_TEST_BINARIES} )
    set_property( TARGET ${test} PROPERTY CXX_STANDARD 11 )
    set_property( TARGET ${test} PROPERTY CXX_STANDARD_REQUIRED ON )
    set_property( TARGET ${test} PROPERTY CXX_EXTENSIONS OFF )
    target_include_directories( ${test} PRIVATE ${SINGLE_INCLUDE_PATH} )
endforeach()

