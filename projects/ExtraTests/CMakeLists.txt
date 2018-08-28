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
add_test(NAME FallbackStringifier COMMAND FallbackStringifier -r compact -s)
set_tests_properties(FallbackStringifier PROPERTIES PASS_REGULAR_EXPRESSION "foo{} for: { !!! }")


set_property( TARGET FallbackStringifier PROPERTY CXX_STANDARD 11 )
set_property( TARGET FallbackStringifier PROPERTY CXX_STANDARD_REQUIRED ON )
set_property( TARGET FallbackStringifier PROPERTY CXX_EXTENSIONS OFF )

target_include_directories( FallbackStringifier PRIVATE ${SINGLE_INCLUDE_PATH} )
target_compile_definitions( FallbackStringifier PRIVATE CATCH_CONFIG_FALLBACK_STRINGIFIER=fallbackStringifier )
