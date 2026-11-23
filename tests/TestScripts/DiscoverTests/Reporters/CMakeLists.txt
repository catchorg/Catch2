cmake_minimum_required(VERSION 3.19)
project(discover-test-reporters LANGUAGES CXX)

add_subdirectory(${CATCH2_PATH} catch2-build)
enable_testing()
include(Catch)

foreach(mode PRE_TEST POST_BUILD file_only stdout_only)
  add_executable(reporter_${mode} reporter-tests.cpp)
  target_link_libraries(reporter_${mode} PRIVATE Catch2::Catch2WithMain)
  set(reporter_options)
  if(NOT mode STREQUAL "file_only")
    list(APPEND reporter_options
      STDOUT_REPORTER console::colour-mode=ansi
    )
  endif()
  if(mode STREQUAL "PRE_TEST" OR mode STREQUAL "POST_BUILD")
    list(APPEND reporter_options DISCOVERY_MODE ${mode})
  endif()
  if(NOT mode STREQUAL "stdout_only")
    list(APPEND reporter_options
      REPORTER junit
      OUTPUT_DIR "reporter output/${mode}"
      OUTPUT_PREFIX "result-"
      OUTPUT_SUFFIX ".xml"
    )
  endif()
  catch_discover_tests(reporter_${mode}
    ${reporter_options}
    TEST_PREFIX "${mode}: "
    EXTRA_ARGS --success
  )
endforeach()
