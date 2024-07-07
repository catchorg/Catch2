// SPDX-License-Identifier: BSL-1.0

// Fixture.cpp

// Catch has three ways to express fixtures:
// - Sections
// - Traditional class-based fixtures that are created and destroyed on every partial run
// - Traditional class-based fixtures that are created at the start of a test case and destroyed at the end of a test case (this file)

// main() provided by linkage to Catch2WithMain

#include <catch2/catch_test_macros.hpp>

struct MyFixture{
    int MyInt = 0;
};

TEST_CASE_FIXTURE(MyFixture, "Tests with MyFixture") {

    const int val = MyInt++;

    SECTION("First partial run") {
        REQUIRE(val == 0);
    }

    SECTION("Second partial run") {
        REQUIRE(val == 1);
    }
}