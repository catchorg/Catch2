// Provide a basis to check whether section selection via the `-c` command line
// option works as intended.

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

bool previouslyRun = false;
bool previouslyRunNested = false;

TEST_CASE( "#1394" ) {
    // -- Don't re-run after specified section is done
    REQUIRE(previouslyRun == false);

    SECTION( "RunSection" ) {
        previouslyRun = true;
    }
    SECTION( "SkipSection" ) {
        // cause an error if this section is called because it shouldn't be
        REQUIRE(1 == 0);
    }
}

TEST_CASE( "#1394 nested" ) {
    REQUIRE(previouslyRunNested == false);

    SECTION( "NestedRunSection" ) {
        SECTION( "s1" ) {
            previouslyRunNested = true;
        }
    }
    SECTION( "NestedSkipSection" ) {
        // cause an error if this section is called because it shouldn't be
        REQUIRE(1 == 0);
    }
}
