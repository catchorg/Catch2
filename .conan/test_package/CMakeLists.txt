cmake_minimum_required(VERSION 3.2.0)
project(test_package CXX)

# We set it only for the convenience of calling the executable
# in the package test function
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

find_package(Catch2 REQUIRED CONFIG)

add_executable(${PROJECT_NAME} test_package.cpp)
# Note: Conan 1.21 doesn't support granular target generation yet.
# The Main library would be included into the unified target.
# It's controlled by the `with_main` option in the recipe.
target_link_libraries(${PROJECT_NAME} Catch2::Catch2)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 14)
