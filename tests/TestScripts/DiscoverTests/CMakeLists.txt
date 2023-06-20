cmake_minimum_required(VERSION 3.10)

project(discover-tests-test
  LANGUAGES CXX
)

add_executable(tests
  register-tests.cpp
)

add_subdirectory(${CATCH2_PATH} catch2-build)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(tests)
