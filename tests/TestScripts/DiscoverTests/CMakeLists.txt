cmake_minimum_required(VERSION 3.16)
project(discover-tests-test LANGUAGES CXX)

add_executable(tests register-tests.cpp)
add_subdirectory(${CATCH2_PATH} catch2-build)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
set_property(TARGET tests PROPERTY CROSSCOMPILING_EMULATOR
  "${CMAKE_COMMAND};-E;env;CATCH2_ENV_EMULATOR=yes"
)

enable_testing()
include(Catch)
set(extra_args)
if(CMAKE_VERSION GREATER_EQUAL 3.27)
  list(APPEND extra_args
    DL_PATHS "${CMAKE_CURRENT_LIST_DIR};${CMAKE_CURRENT_LIST_DIR}/.."
  )
endif()
if(NOT TEST_DISCOVERY_MODE)
  set(TEST_DISCOVERY_MODE PRE_TEST)
endif()
catch_discover_tests(
  tests
  TEST_PREFIX " prefix "
  TEST_SUFFIX " suffix "
  ADD_TAGS_AS_LABELS
  DISCOVERY_MODE ${TEST_DISCOVERY_MODE}
  PROPERTIES
    ENVIRONMENT "CATCH2_ENV_FIRST=one;CATCH2_ENV_SECOND=two;CATCH2_ENV_SEMICOLON=left\\;right;CATCH2_ENV_EMPTY="
    PROCESSORS 3
    TIMEOUT 10
  ${extra_args}
)

# DISCOVERY_MODE <POST_BUILD|PRE_TEST>
