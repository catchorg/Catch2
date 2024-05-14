# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

function(add_command NAME)
  set(_args "")
  # use ARGV* instead of ARGN, because ARGN splits arrays into multiple arguments
  math(EXPR _last_arg ${ARGC}-1)
  foreach(_n RANGE 1 ${_last_arg})
    set(_arg "${ARGV${_n}}")
    if(_arg MATCHES "[^-./:a-zA-Z0-9_]")
      set(_args "${_args} [==[${_arg}]==]") # form a bracket_argument
    else()
      set(_args "${_args} ${_arg}")
    endif()
  endforeach()
  set(script "${script}${NAME}(${_args})\n" PARENT_SCOPE)
endfunction()

function(remove_brackets VAR VAR_OUT)
  string(REGEX MATCH "^\\[=*\\[" start_bracket "${VAR}")
  string(REPLACE "[" "]" end_bracket "${start_bracket}")
  string(REPLACE "[" "\\[" start_bracket "${start_bracket}")
  string(REPLACE "]" "\\]" end_bracket "${end_bracket}")
  string(REGEX REPLACE "^${start_bracket}" "" new_var "${VAR}")
  string(REGEX REPLACE "${end_bracket}\$" "" new_var "${new_var}")
  set(${VAR_OUT} "${new_var}" PARENT_SCOPE)
endfunction()

function(catch_discover_tests_impl)
  # Don't ignore empty elements in a list
  cmake_policy(SET CMP0007 NEW)

  cmake_parse_arguments(
    ""
    ""
    "TEST_EXECUTABLE;TEST_WORKING_DIR;TEST_OUTPUT_DIR;TEST_OUTPUT_PREFIX;TEST_OUTPUT_SUFFIX;TEST_PREFIX;TEST_REPORTER;TEST_SPEC;TEST_SUFFIX;TEST_LIST;CTEST_FILE"
    "TEST_EXTRA_ARGS;TEST_PROPERTIES;TEST_EXECUTOR;TEST_DL_PATHS"
    ${ARGN}
  )

  set(prefix "${_TEST_PREFIX}")
  set(suffix "${_TEST_SUFFIX}")
  set(spec ${_TEST_SPEC})
  set(extra_args ${_TEST_EXTRA_ARGS})
  set(properties ${_TEST_PROPERTIES})
  set(reporter ${_TEST_REPORTER})
  set(output_dir ${_TEST_OUTPUT_DIR})
  set(output_prefix ${_TEST_OUTPUT_PREFIX})
  set(output_suffix ${_TEST_OUTPUT_SUFFIX})
  set(dl_paths ${_TEST_DL_PATHS})
  set(script)
  set(suite)
  set(tests)

  if(WIN32)
    set(dl_paths_variable_name PATH)
  elseif(APPLE)
    set(dl_paths_variable_name DYLD_LIBRARY_PATH)
  else()
    set(dl_paths_variable_name LD_LIBRARY_PATH)
  endif()

  # Run test executable to get list of available tests
  if(NOT EXISTS "${_TEST_EXECUTABLE}")
    message(FATAL_ERROR
      "Specified test executable '${_TEST_EXECUTABLE}' does not exist"
    )
  endif()

  if(dl_paths)
    cmake_path(CONVERT "${dl_paths}" TO_NATIVE_PATH_LIST paths)
    set(ENV{${dl_paths_variable_name}} "${paths}")
  endif()

  execute_process(
    COMMAND ${_TEST_EXECUTOR} "${_TEST_EXECUTABLE}" ${spec} --list-tests --reporter cmake
    OUTPUT_VARIABLE test_cases
    RESULT_VARIABLE result
    WORKING_DIRECTORY "${_TEST_WORKING_DIR}"
  )
  if(NOT ${result} EQUAL 0)
    message(FATAL_ERROR
      "Error running test executable '${_TEST_EXECUTABLE}':\n"
      "  Result: ${result}\n"
      "  Output: ${output}\n"
    )
  endif()

  # Prepare reporter
  if(reporter)
    set(reporter_arg "--reporter ${reporter}")

    # Run test executable to check whether reporter is available
    # note that the use of --list-reporters is not the important part,
    # we only want to check whether the execution succeeds with ${reporter_arg}
    execute_process(
      COMMAND ${_TEST_EXECUTOR} "${_TEST_EXECUTABLE}" ${spec} ${reporter_arg} --list-reporters
      OUTPUT_VARIABLE reporter_check_output
      RESULT_VARIABLE reporter_check_result
      WORKING_DIRECTORY "${_TEST_WORKING_DIR}"
    )
    if(${reporter_check_result} EQUAL 255)
      message(FATAL_ERROR
        "\"${reporter}\" is not a valid reporter!\n"
      )
    elseif(NOT ${reporter_check_result} EQUAL 0)
      message(FATAL_ERROR
        "Error running test executable '${_TEST_EXECUTABLE}':\n"
        "  Result: ${reporter_check_result}\n"
        "  Output: ${reporter_check_output}\n"
      )
    endif()
  endif()

  # Prepare output dir
  if(output_dir AND NOT IS_ABSOLUTE ${output_dir})
    set(output_dir "${_TEST_WORKING_DIR}/${output_dir}")
    if(NOT EXISTS ${output_dir})
      file(MAKE_DIRECTORY ${output_dir})
    endif()
  endif()

  if(dl_paths)
    foreach(path ${dl_paths})
      cmake_path(NATIVE_PATH path native_path)
      list(APPEND environment_modifications "${dl_paths_variable_name}=path_list_prepend:${native_path}")
    endforeach()
  endif()

  remove_brackets("${test_cases}" test_cases)
  foreach(test_case ${test_cases})
    # Remove long form brackets
    # [[...test case list...]]
    remove_brackets("${test_case}" test_case)

    # Extract test name and tags
    list(GET test_case 0 raw_test_name)
    list(GET test_case 4 tags)

    # Remove long form brackets from test name
    # [=[...test name...]=]
    remove_brackets("${raw_test_name}" raw_test_name)
    set(test_name "${raw_test_name}")

    # Escape characters in test case names that would be parsed by Catch2
    # Note that the \ escaping must happen FIRST! Do not change the order.
    foreach(char \\ , [ ])
      string(REPLACE ${char} "\\${char}" test_name "${test_name}")
    endforeach(char)

    # ...add output dir
    if(output_dir)
      string(REGEX REPLACE "[^A-Za-z0-9_]" "_" test_name_clean "${test_name}")
      set(output_dir_arg "--out ${output_dir}/${output_prefix}${test_name_clean}${output_suffix}")
    endif()

    # ... add to script
    add_command(add_test
      "${prefix}${raw_test_name}${suffix}"
      ${_TEST_EXECUTOR}
      "${_TEST_EXECUTABLE}"
      "${test_name}"
      ${extra_args}
      "${reporter_arg}"
      "${output_dir_arg}"
    )
    add_command(set_tests_properties
      "${prefix}${raw_test_name}${suffix}"
      PROPERTIES
      WORKING_DIRECTORY "${_TEST_WORKING_DIR}"
      ${properties}
    )

    # ... add any environment modifications
    if(environment_modifications)
      add_command(set_tests_properties
        "${prefix}${raw_test_name}${suffix}"
        PROPERTIES
        ENVIRONMENT_MODIFICATION "${environment_modifications}"
      )
    endif()

    # Remove long form brackets from tags list
    # [=[...tags list...]=]
    string(LENGTH "${tags}" tags_length)
    if("${tags_length}" GREATER 0)
      remove_brackets("${tags}" tags)

      # ... and any tags as labels
      foreach(tag ${tags})
        # Remove long form brackets from tag
        # [==[...tags list...]==]
        remove_brackets("${tag}" tag)

        add_command(set_tests_properties
          "${prefix}${raw_test_name}${suffix}"
          PROPERTIES
            LABELS "${tag}"
        )
      endforeach()
    endif()

    list(APPEND tests "${prefix}${raw_test_name}${suffix}")
  endforeach()

  # Create a list of all discovered tests, which users may use to e.g. set
  # properties on the tests
  add_command(set ${_TEST_LIST} ${tests})

  # Write CTest script
  file(WRITE "${_CTEST_FILE}" "${script}")
endfunction()

if(CMAKE_SCRIPT_MODE_FILE)
  catch_discover_tests_impl(
    TEST_EXECUTABLE ${TEST_EXECUTABLE}
    TEST_EXECUTOR ${TEST_EXECUTOR}
    TEST_WORKING_DIR ${TEST_WORKING_DIR}
    TEST_SPEC ${TEST_SPEC}
    TEST_EXTRA_ARGS ${TEST_EXTRA_ARGS}
    TEST_PROPERTIES ${TEST_PROPERTIES}
    TEST_PREFIX ${TEST_PREFIX}
    TEST_SUFFIX ${TEST_SUFFIX}
    TEST_LIST ${TEST_LIST}
    TEST_REPORTER ${TEST_REPORTER}
    TEST_OUTPUT_DIR ${TEST_OUTPUT_DIR}
    TEST_OUTPUT_PREFIX ${TEST_OUTPUT_PREFIX}
    TEST_OUTPUT_SUFFIX ${TEST_OUTPUT_SUFFIX}
    TEST_DL_PATHS ${TEST_DL_PATHS}
    CTEST_FILE ${CTEST_FILE}
  )
endif()
