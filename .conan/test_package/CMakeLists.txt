cmake_minimum_required(VERSION 3.2.0)
project(test_package CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Catch2 REQUIRED CONFIG)

add_executable(${PROJECT_NAME} test_package.cpp)
target_link_libraries(${PROJECT_NAME} CONAN_PKG::Catch2)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)
