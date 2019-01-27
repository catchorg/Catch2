#include "catch.hpp"

#include <cstring>


// Generators and sections can be nested freely
TEST_CASE("Generators -- simple", "[generators]") {
    auto i = GENERATE(1, 2, 3);
    SECTION("one") {
        auto j = GENERATE(values({ -3, -2, -1 }));
        REQUIRE(j < i);
    }

    SECTION("two") {
        // You can also explicitly set type for generators via Catch::Generators::as
        auto str = GENERATE(as<std::string>{}, "a", "bb", "ccc");
        REQUIRE(4u * i > str.size());
    }
}

// You can create a cartesian-product of generators by creating multiple ones
TEST_CASE("3x3x3 ints", "[generators]") {
    auto x = GENERATE(1, 2, 3);
    auto y = GENERATE(4, 5, 6);
    auto z = GENERATE(7, 8, 9);
    // These assertions will be run 27 times (3x3x3)
    CHECK(x < y);
    CHECK(y < z);
    REQUIRE(x < z);
}

// You can also create data tuples
TEST_CASE("tables", "[generators]") {
    // Note that this will not compile with libstdc++ older than libstdc++6
    // See https://stackoverflow.com/questions/12436586/tuple-vector-and-initializer-list
    // for possible workarounds
    //    auto data = GENERATE(table<char const*, int>({
    //        {"first", 5},
    //        {"second", 6},
    //        {"third", 5},
    //        {"etc...", 6}
    //    }));

    // Workaround for the libstdc++ bug mentioned above
    using tuple_type = std::tuple<char const*, int>;
    auto data = GENERATE(table<char const*, int>({
        tuple_type{"first", 5},
        tuple_type{"second", 6},
        tuple_type{"third", 5},
        tuple_type{"etc...", 6}
    }));

    REQUIRE(strlen(std::get<0>(data)) == static_cast<size_t>(std::get<1>(data)));
}


#ifdef __cpp_structured_bindings

// One way to do pairs of values (actual/ expected?)
// For a simple case like this I'd recommend writing out a series of REQUIREs
// but it demonstrates a possible usage.
// Spelling out the pair like this is a bit verbose, so read on for better examples
// - the use of structured bindings here is an optional convenience
TEST_CASE( "strlen", "[approvals][generators]" ) {
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
TEST_CASE( "strlen2", "[approvals][generators]" ) {
    auto [test_input, expected] = GENERATE( table<std::string, size_t>({
            {"one", 3},
            {"two", 3},
            {"three", 5},
            {"four", 4}
        }));

    REQUIRE( test_input.size() == expected );
}
#endif


// An alternate way of doing data tables without structured bindings
struct Data { std::string str; size_t len; };

TEST_CASE( "strlen3", "[generators]" ) {
    auto data = GENERATE( values<Data>({
            {"one", 3},
            {"two", 3},
            {"three", 5},
            {"four", 4}
        }));

    REQUIRE( data.str.size() == data.len );
}



#ifdef __cpp_structured_bindings

// Based on example from https://docs.cucumber.io/gherkin/reference/#scenario-outline
// (thanks to https://github.com/catchorg/Catch2/issues/850#issuecomment-399504851)

// Note that GIVEN, WHEN, and THEN now forward onto DYNAMIC_SECTION instead of SECTION.
// DYNAMIC_SECTION takes its name as a stringstream-style expression, so can be formatted using
// variables in scope - such as the generated variables here. This reads quite nicely in the
// test name output (the full scenario description).

static auto eatCucumbers( int start, int eat ) -> int { return start-eat; }

SCENARIO("Eating cucumbers", "[generators][approvals]") {

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
