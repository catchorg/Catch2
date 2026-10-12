# SPDX-License-Identifier: BSL-1.0

# Fake Catch2 binary used by benchmark_discovery.py.
#
# catch_discover_tests_impl discovers tests by running
#
#     ${TEST_EXECUTOR} "${TEST_EXECUTABLE}" ... --list-tests --reporter json --out <file>
#
# and then reading the JSON listing back from <file>. This script fakes
# being Catch2 binary by taking the fake JSON listing prepared by
# `benchmark_discovery.py` and copying it over to the file path specified
# after the `--out` argument.
#
# The fake JSON listing is passed as BENCH_LISTING CMake argument.

if(NOT DEFINED BENCH_LISTING)
  message(FATAL_ERROR "copy_shim: BENCH_LISTING is not set")
endif()
if(NOT EXISTS "${BENCH_LISTING}")
  message(FATAL_ERROR "copy_shim: BENCH_LISTING '${BENCH_LISTING}' does not exist")
endif()


# FIXME: better handling of bad --out
# Find the "--out <file>" argument pair among the forwarded arguments.
set(_out_file "")
math(EXPR _last "${CMAKE_ARGC} - 1")
foreach(_i RANGE 0 ${_last})
  if("${CMAKE_ARGV${_i}}" STREQUAL "--out")
    math(EXPR _next "${_i} + 1")
    set(_out_file "${CMAKE_ARGV${_next}}")
    break()
  endif()
endforeach()

if(_out_file STREQUAL "")
  message(FATAL_ERROR "copy_shim: could not find '--out <file>' in arguments")
endif()

file(COPY_FILE "${BENCH_LISTING}" "${_out_file}")
