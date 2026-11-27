cmake_minimum_required(VERSION 3.16)
project(discover-tests-test LANGUAGES CXX)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_executable(tests register-tests.cpp)
add_subdirectory(${CATCH2_PATH} catch2-build)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# A Windows test executable run under Wine treats a leading '/' as an option
# prefix. Use a wrapper that reproduces that behavior so discovery is covered
# without requiring Wine. See #3251.
set_target_properties(tests PROPERTIES
  CROSSCOMPILING_EMULATOR "${Python3_EXECUTABLE};${CMAKE_CURRENT_LIST_DIR}/windows-arg-emulator.py"
)

enable_testing()
include(Catch)
set(extra_args)
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
  DISCOVERY_MODE PRE_TEST
  ${extra_args}
)

# DISCOVERY_MODE <POST_BUILD|PRE_TEST>
