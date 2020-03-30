// 000-CatchMain.cpp

// It is generally recommended to have a single file provide the main
// of a testing binary, and other test files to link against it.

// Let Catch provide main():
#include <catch2/internal/catch_default_main.hpp>

// That's it

// Compile implementation of Catch for use with files that do contain tests:
// - g++ -std=c++11 -Wall -I$(CATCH_SINGLE_INCLUDE) -c 000-CatchMain.cpp
// - cl -EHsc -I%CATCH_SINGLE_INCLUDE% -c 000-CatchMain.cpp
