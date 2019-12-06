cmake_minimum_required(VERSION 3.0)

project(CatchCoverageHelper)

add_executable(CoverageHelper coverage-helper.cpp)
set_property(TARGET CoverageHelper PROPERTY CXX_STANDARD 11)
set_property(TARGET CoverageHelper PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET CoverageHelper PROPERTY CXX_EXTENSIONS OFF)
if (MSVC)
    target_compile_options( CoverageHelper PRIVATE /W4 /w44265 /WX /w44061 /w44062 )
endif()
