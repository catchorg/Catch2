// 200-Rpt-CatchMain.cpp

// In a Catch project with multiple files, dedicate one file to compile the
// source code of Catch itself and reuse the resulting object file for linking.

// Let Catch provide main():
#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#ifdef   CATCH_EXAMPLE_RPT_1
#include CATCH_EXAMPLE_RPT_1
#endif

#ifdef   CATCH_EXAMPLE_RPT_2
#include CATCH_EXAMPLE_RPT_2
#endif

#ifdef   CATCH_EXAMPLE_RPT_3
#include CATCH_EXAMPLE_RPT_3
#endif

// That's it

// Compile implementation of Catch for use with files that do contain tests:
// - g++ -std=c++11 -Wall -I$(CATCH_ROOT) -DCATCH_EXAMPLE_RPT_1=\"include/reporters/catch_reporter_teamcity.hpp\" -o 200-Rpt-CatchMainTeamCity.o -c 200-Rpt-CatchMain.cpp
// cl -EHsc -I%CATCH_ROOT% -DCATCH_EXAMPLE_RPT_1=\"include/reporters/catch_reporter_teamcity.hpp\" -Fo200-Rpt-CatchMainTeamCity.obj -c 200-Rpt-CatchMain.cpp
