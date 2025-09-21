
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

// 302-Gen-Table.cpp
// Shows how to use table to run a test many times with different inputs. Lifted from examples on
// issue #850.

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <string>

struct TestSubject {
    // this is the method we are going to test. It returns the length of the
    // input string.
    size_t GetLength( const std::string& input ) const { return input.size(); }
};


TEST_CASE("Table allows pre-computed test inputs and outputs", "[example][generator]") {
    // do setup here as normal
    TestSubject subj;

    SECTION("This section is run for each row in the table") {
        const auto [test_input, expected_output] =
            GENERATE( table<std::string, size_t>(
                { /* In this case one of the parameters to our test case is the
                   * expected output, but this is not required. There could be
                   * multiple expected values in the table, which can have any
                   * (fixed) number of columns.
                   */
                  { "one", 3 },
                  { "two", 3 },
                  { "three", 5 },
                  { "four", 4 } } ) );

        // run the test
        auto result = subj.GetLength(test_input);
        // capture the input data to go with the outputs.
        CAPTURE(test_input);
        // check it matches the pre-calculated data
        REQUIRE(result == expected_output);
    }   // end section
}

// Compiling and running this file will result in 4 successful assertions
