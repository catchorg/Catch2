# SPDX-License-Identifier: BSL-1.0

# Unit tests for `split_json_array` helper in `extras/CatchAddTests.cmake`.
#
# Yes, we are at the stage where the script helpers need unit tests.
#
# Run as
#     cmake -DCATCH_ADD_TESTS_SCRIPT=/path/to/extras/CatchAddTests.cmake \
#           -P TestDecomposeJsonArray.cmake


cmake_minimum_required(VERSION 3.19)

if(NOT DEFINED CATCH_ADD_TESTS_SCRIPT)
  message(FATAL_ERROR "Missing argument `CATCH_ADD_TESTS_SCRIPT`")
endif()

if(NOT EXISTS "${CATCH_ADD_TESTS_SCRIPT}")
  message(FATAL_ERROR "Cannot find CatchAddTests.cmake at '${CATCH_ADD_TESTS_SCRIPT}'")
endif()

# Pull in the helper functions. Without `TEST_EXECUTABLE` being defined,
# `catch_discover_tests_impl` is not called.
include("${CATCH_ADD_TESTS_SCRIPT}")

set(_failures 0)

# Parses out test names from provided listings and returns them through `out_var`.
# Semicolons in test names are escaped as `@SEMI@`.
function(decomposed_names listing_var out_var)
  split_json_array(${listing_var} elements)
  set(names "")
  foreach(element IN LISTS elements)
    magic_unescape_chars(element)
    string(JSON name ERROR_VARIABLE err GET "${element}" "name")
    if(NOT err STREQUAL "NOTFOUND")
      set(${out_var} "PARSE-ERROR" PARENT_SCOPE)
      return()
    endif()
    string(REPLACE ";" "@SEMI@" name "${name}")
    list(APPEND names "${name}")
  endforeach()
  set(${out_var} "${names}" PARENT_SCOPE)
endfunction()

# Assert that decomposing the provided listing returns the expected list
# of (test) names.
# Semicolons inside expected names must be escaped as `@SEMI@`.
function(expect_names description listing_var expected_names)
  decomposed_names(${listing_var} actual_names)
  if(actual_names STREQUAL expected_names)
    message("  [PASS] ${description}")
  else()
    message("  [FAIL] ${description}")
    message("         expected: ${expected_names}")
    message("         actual:   ${actual_names}")
    math(EXPR _n "${_failures} + 1")
    set(_failures "${_n}" PARENT_SCOPE)
  endif()
endfunction()

# Assert that the tags in one decomposed element are preserved exactly.
# Semicolons in expected tags have to be escaped as `@SEMI@`.
function(expect_tags description listing_var element_index expected_tags)
  split_json_array(${listing_var} elements)
  list(GET elements ${element_index} element)
  magic_unescape_chars(element)

  string(JSON tags ERROR_VARIABLE err GET "${element}" "tags")
  if(NOT err STREQUAL "NOTFOUND")
    set(actual_tags "PARSE-ERROR")
  else()
    string(JSON tag_count LENGTH "${tags}")
    set(actual_tags "")
    if(tag_count GREATER 0)
      math(EXPR last_tag "${tag_count} - 1")
      foreach(tag_index RANGE ${last_tag})
        string(JSON tag GET "${tags}" ${tag_index})
        string(REPLACE ";" "@SEMI@" tag "${tag}")
        list(APPEND actual_tags "${tag}")
      endforeach()
    endif()
  endif()

  if(actual_tags STREQUAL expected_tags)
    message("  [PASS] ${description}")
  else()
    message("  [FAIL] ${description}")
    message("         expected: ${expected_tags}")
    message("         actual:   ${actual_tags}")
    math(EXPR _n "${_failures} + 1")
    set(_failures "${_n}" PARENT_SCOPE)
  endif()
endfunction()

# Convenience for building a minimal-but-realistic (pretty-printed) listing.
function(make_listing out_var)
  set(objects "")
  foreach(name IN LISTS ARGN)
    # Build each object by hand; the names passed in are already JSON-safe.
    string(APPEND objects
      "  {\n"
      "    \"class-name\" : \"\",\n"
      "    \"name\" : \"${name}\",\n"
      "    \"tags\" : [ \"[tag]\" ]\n"
      "  },\n")
  endforeach()
  string(REGEX REPLACE ",\n$" "\n" objects "${objects}")
  set(${out_var} "[\n${objects}]" PARENT_SCOPE)
endfunction()

message(STATUS "Running split_json_array correctness tests")

# There are 2 main difficulties in the array decomposition that we need
# to check for:
# 1) Names/tags that contain the expected element boundary (`}<ws>*,<ws>*{`)
#    inside them, and thus are split into invalid JSON.
# 2) Names/tags that contain CMake-relevant characters (e.g. semicolon,
#    which is list separator) and thus cause issues when processing the
#    string splits.

make_listing(listing "")
expect_names("No tests" listing "")

make_listing(listing "plain")
expect_names("Single test" listing "plain")

make_listing(listing "n1" "n2" "n3")
expect_names("Multiple tests" listing "n1;n2;n3")

make_listing(listing "before },{ after" "second")
expect_names("The element boundary in a test name" listing "before },{ after;second")

make_listing(listing "},{" "next")
expect_names("Test name is just the boundary" listing "},{;next")

make_listing(listing "a},{b},{c" "x" "y},{z")
expect_names("Test name has multiple boundaries" listing "a},{b},{c;x;y},{z")

# Listings with semicolons have to be built by hand, or CMake would mess
# them up before decomposition.
set(listing "[{\"name\":\"has;semicolon\",\"tags\":[]},{\"name\":\"and;another;one\",\"tags\":[]}]")
expect_names("Test names with semicolons" listing "has@SEMI@semicolon;and@SEMI@another@SEMI@one")

set(listing "[{\"name\":\"C:\\\\path\\\\file\",\"tags\":[]},{\"name\":\"plain\",\"tags\":[]}]")
expect_names("Test names with backslashes" listing "C:\\path\\file;plain")

set(listing "[{\"name\":\"compact1\",\"tags\":[]},{\"name\":\"compact2\",\"tags\":[]}]")
expect_names("compact json" listing "compact1;compact2")

make_listing(listing "Then } , { we }\t,\t{ concatenate } , { them } ,{back},{" "second")
expect_names("Whitespace around commas in test names survive split"
  listing "Then } , { we }\t,\t{ concatenate } , { them } ,{back},{;second")

make_listing(listing "}},{{" "second")
expect_names("Doubled up boundary braces in names" listing "}},{{;second")

# Square brackets and array likes in the test names.
set(listing "[{\"name\":\"Arrays [{},{}] wheee\",\"tags\":[\"also},{tags\",\"tag;with;semicolons\"]},{\"name\":\"n\",\"tags\":[]}]")
expect_names("array-like substring in name" listing "Arrays [{},{}] wheee;n")
expect_tags("boundary-like and semicolon tags are preserved" listing 0
  "also},{tags;tag@SEMI@with@SEMI@semicolons")
expect_tags("empty tags are preserved" listing 1 "")

# Listings with square brackets have to be built by hand, or CMake would
# mess them up before decomposition.
set(listing "[{\"name\":\"[\",\"tags\":[]},{\"name\":\"middle\",\"tags\":[]},{\"name\":\"]\",\"tags\":[]}]")
expect_names("unmatched square brackets" listing "[;middle;]")

set(listing "[{\"name\":\"a[b]c\",\"tags\":[]},{\"name\":\"[open\",\"tags\":[]},{\"name\":\"close]\",\"tags\":[]},{\"name\":\"[]\",\"tags\":[]}]")
expect_names("Test names with mess of brackets" listing "a[b]c;[open;close];[]")

# Without careful handling, these could be evaluated as variables.
make_listing(listing
  "curly \${NOT_A_VAR}"
  "env \$ENV{HOME}"
  "cache \$CACHE{FOO}"
  "genex \$<CONFIG>"
  "bare \$ and \$\$ and \${ unterminated")
expect_names("Test names with dollars and various brackets (CMake vars)" listing
  "curly \${NOT_A_VAR};env \$ENV{HOME};cache \$CACHE{FOO};genex \$<CONFIG>;bare \$ and \$\$ and \${ unterminated")

# Listings with semicolons have to be built by hand, or CMake would mess
# them up before decomposition.
# This is just a huge mess of everything to see if anything shakes loose.
set(listing "[{\"name\":\"\$ENV{X};[weird]{},{ mix \$<0:no> \${VAR} };,{ }},{{\",\"tags\":[]},{\"name\":\"after\",\"tags\":[]}]")
expect_names("combined mega-case" listing
  "\$ENV{X}@SEMI@[weird]{},{ mix \$<0:no> \${VAR} }@SEMI@,{ }},{{;after")

if(_failures GREATER 0)
  message(FATAL_ERROR "${_failures} decomposition test(s) failed")
endif()

message(STATUS "All split_json_array correctness tests passed")
