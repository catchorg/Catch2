cmake_minimum_required(VERSION 3.0)
project(CatchTest CXX)

# Conan setup ##################################################################
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include(CTest)

# Build Test ###################################################################
file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

# Execute Test #################################################################
enable_testing()
add_test(NAME test-single-header
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
         COMMAND ${CMAKE_PROJECT_NAME})
