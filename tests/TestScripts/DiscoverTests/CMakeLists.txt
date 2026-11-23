cmake_minimum_required(VERSION 3.16)
project(discover-tests-test LANGUAGES CXX)

add_executable(tests register-tests.cpp)
add_subdirectory(${CATCH2_PATH} catch2-build)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

enable_testing()
include(Catch)
set(extra_args)
if(DISCOVERY_USE_STDOUT)
  list(APPEND extra_args USE_STDOUT)
  target_compile_definitions(tests PRIVATE DISCOVERY_USE_STDOUT)
endif()
if(NOT DISCOVERY_MODE)
  set(DISCOVERY_MODE PRE_TEST)
endif()
if(CMAKE_VERSION GREATER_EQUAL 3.27)
  list(APPEND extra_args
    DL_PATHS "${CMAKE_CURRENT_LIST_DIR};${CMAKE_CURRENT_LIST_DIR}/.."
  )
endif()
catch_discover_tests(
  tests
  TEST_PREFIX " prefix "
  TEST_SUFFIX " suffix "
  ADD_TAGS_AS_LABELS
  DISCOVERY_MODE ${DISCOVERY_MODE}
  ${extra_args}
)

# DISCOVERY_MODE <POST_BUILD|PRE_TEST>
