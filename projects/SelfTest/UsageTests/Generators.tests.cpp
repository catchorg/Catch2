#include "catch.hpp"

// Examples of usage of Generators

// This test doesn't do much - it just shows how you can have several generators, of different
// types (ie `i` and `j` are different types), can be sequenced using `,` and
// can be expressed as named generators (like range) or as individual values.
// Generators can be mixed with SECTIONs.
// At time of writing the generated values are not automatically reported as part of the test
// name or associated values - so we explicitly CAPTURE then (run this with `-s` to see them).
// We could also incorporate them into the section names using DYNAMIC_SECTION. See the BDD
// example later for more information.
TEST_CASE("Generators") {

    auto i = GENERATE( as<std::string>(), "a", "b", "c" );

    SECTION( "one" ) {
        auto j = GENERATE( range( 8, 11 ), 2 );

        CAPTURE( i, j );
        SUCCEED();
    }
    SECTION( "two" ) {
        auto j = GENERATE( 3.141, 1.379 );
        CAPTURE( i, j );
        SUCCEED();
    }
}

// This one generates the cross-product of two ranges.
// It's mostly here to demonstrate the performance which, at time of writing,
// leaves a lot to be desired.
TEST_CASE( "100x100 ints", "[.][approvals]" ) {
    auto x = GENERATE( range( 0,100 ) );
    auto y = GENERATE( range( 200,300 ) );

    CHECK( x < y );
}

// smaller version
TEST_CASE( "10x10 ints" ) {
    auto x = GENERATE( range( 1,11 ) );
    auto y = GENERATE( range( 101, 111 ) );

    CHECK( x < y );
}

// Some of the following tests use structured bindings for convenience and so are
// conditionally compiled using the de-facto (and soon to be formally) standard
// feature macros

#ifdef __cpp_structured_bindings

// One way to do pairs of values (actual/ expected?)
// For a simple case like this I'd recommend writing out a series of REQUIREs
// but it demonstrates a possible usage.
// Spelling out the pair like this is a bit verbose, so read on for better examples
// - the use of structured bindings here is an optional convenience
TEST_CASE( "strlen", "[approvals]" ) {
    auto [test_input, expected] = GENERATE( values<std::pair<std::string_view, size_t>>({
            {"one", 3},
            {"two", 3},
            {"three", 5},
            {"four", 4}
        }));

    REQUIRE( test_input.size() == expected );
}

// A nicer way to do pairs (or more) of values - using the table generator.
// Note, you must specify the types up-front.
TEST_CASE( "strlen2", "[approvals]" ) {
    auto [test_input, expected] = GENERATE( table<std::string, size_t>({
            {"one", 3},
            {"two", 3},
            {"three", 5},
            {"four", 4}
        }));

    REQUIRE( test_input.size() == expected );
}
#endif

// An alternate way of doing data tables without structure bindings
// - I'd prefer to have the Data class within the test case but gcc 4.x doesn't seem to like it
struct Data { std::string str; size_t len; };

TEST_CASE( "strlen3" ) {
    auto data = GENERATE( values<Data>({
            {"one", 3},
            {"two", 3},
            {"three", 5},
            {"four", 4}
        }));

    REQUIRE( data.str.size() == data.len );
}

// A nod towards property-based testing - generate a random selection of numbers
// in a range and assert on global properties those numbers.
static auto square( int i ) -> int { return i*i; }

TEST_CASE( "Random numbers in a range", "[.][approvals]" ) {
    auto x = GENERATE( random( -10000, 10000 ) );
    CAPTURE( x );
    REQUIRE( square(x) >= 0 );
}

#ifdef __cpp_structured_bindings

// Based on example from https://docs.cucumber.io/gherkin/reference/#scenario-outline
// (thanks to https://github.com/catchorg/Catch2/issues/850#issuecomment-399504851)

// Note that GIVEN, WHEN, and THEN now forward onto DYNAMIC_SECTION instead of SECTION.
// DYNAMIC_SECTION takes its name as a stringstream-style expression, so can be formatted using
// variables in scope - such as the generated variables here. This reads quite nicely in the
// test name output (the full scenario description).

static auto eatCucumbers( int start, int eat ) -> int { return start-eat; }

SCENARIO("Eating cucumbers", "[approvals]") {

    auto [start, eat, left] = GENERATE( table<int,int,int> ({
            { 12, 5, 7 },
            { 20, 5, 15 }
        }));

    GIVEN( "there are " << start << " cucumbers" )
    WHEN( "I eat " << eat << " cucumbers" )
    THEN( "I should have " << left << " cucumbers" ) {
        REQUIRE( eatCucumbers( start, eat ) == left );
    }
}
#endif
