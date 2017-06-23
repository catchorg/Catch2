cmake_minimum_required(VERSION 3.0)
project(CatchTest CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(${CMAKE_PROJECT_NAME} MainTest.cpp)
