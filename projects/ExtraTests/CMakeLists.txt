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

