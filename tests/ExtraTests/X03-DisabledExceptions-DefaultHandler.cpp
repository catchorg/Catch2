#include <catch2/catch_test_macros.hpp>

TEST_CASE("Tests that run") {
    // All of these should be run and be reported
    CHECK(1 == 2);
    CHECK(1 == 1);
    CHECK(1 != 3);
    CHECK(1 == 4);
}



TEST_CASE("Tests that abort") {
    // Avoid abort and other exceptional exits -- there is no way
    // to tell CMake that abort is the desired outcome of a test.
    std::set_terminate([](){exit(1);});
    REQUIRE(1 == 1);
    REQUIRE(1 != 2);
    REQUIRE(1 == 3);
    // We should not get here, because the test above aborts
    REQUIRE(1 != 4);
}
