
#include "catch.hpp"

#include <iostream> // !DBG
#include <numeric>
#include <map>
#include <set>

#include "internal/catch_generators.hpp"

// Tests of implementation details

TEST_CASE("Generators impl") {
    using namespace Catch::generators;

    SECTION( "range" ) {
        auto gen = range(1,3);

        CHECK( gen.size() == 3 );

        CHECK( gen[0] == 1 );
        CHECK( gen[1] == 2 );
        CHECK( gen[2] == 3 );
    }
    SECTION( "fixed values" ) {
        auto gen = values( { 3, 1, 4, 1 } );

        CHECK( gen.size() == 4 );
        CHECK( gen[0] == 3 );
        CHECK( gen[1] == 1 );
        CHECK( gen[2] == 4 );
        CHECK( gen[3] == 1 );
    }
    SECTION( "random range" ) {
        auto gen = random( 3, 8 );

        CHECK( gen.size() == 6 );
        for( size_t i = 0; i < 6; ++i ) {
            CHECK( gen[i] >= 3 );
            CHECK( gen[i] <= 8 );
            if( i > 0 )
                CHECK( gen[i] != gen[i-1] );
        }
    }
    SECTION( "random selection" ) {
        auto gen = random<int>( 10 );

        CHECK( gen.size() == 10 );
        for( size_t i = 0; i < 10; ++i ) {
            if( i > 0 )
                CHECK( gen[i] != gen[i-1] );
        }
    }
    SECTION( "combined" ) {
        auto gen = makeGenerators( range( 1, 2 ), values( { 9, 7 } ) );

        CHECK( gen.size() == 4 );
        CHECK( gen[0] == 1 );
        CHECK( gen[1] == 2 );
        CHECK( gen[2] == 9 );
        CHECK( gen[3] == 7 );
    }

    SECTION( "values" ) {
        auto gen = makeGenerators( 3, 1 );

        CHECK( gen.size() == 2 );
        CHECK( gen[0] == 3 );
        CHECK( gen[1] == 1 );
    }
    SECTION( "values2" ) {
        auto gen = makeGenerators( 3, 1 );

        CHECK( gen.size() == 2 );
        CHECK( gen[0] == 3 );
        CHECK( gen[1] == 1 );
    }


    SECTION( "type erasure" ) {
        auto gen = makeGenerators( range( 7, 9 ), 11 );

        // Make type erased version
        auto dynCopy = pf::make_unique<Generators<int>>( std::move( gen ) );
        std::unique_ptr<GeneratorBase const> base = std::move( dynCopy );

        // Only thing we can do is ask for the size
        CHECK( base->size() == 4 );

        // Restore typed version
        auto typed = dynamic_cast<Generators<int> const*>( base.get() );
        REQUIRE( typed );
        CHECK( typed->size() == 4 );
        CHECK( (*typed)[0] == 7 );
        CHECK( (*typed)[3] == 11 );
    }
}


// The rest are usage tests

TEST_CASE("Generators") {

    auto i = GENERATE( as<std::string>(), "a", "b", "c" );

    SECTION( "one" ) {
        auto j = GENERATE( range( 8, 11 ), 2 );
        std::cout << "one: " << i << ", " << j << std::endl;
    }
    SECTION( "two" ) {
        auto j = GENERATE( 3.141, 1.379 );
        std::cout << "two: " << i << ", " << j << std::endl;
    }
}

// This generates the cross-product of two ranges.
// It's mostly here to demonstrate the performance which, at time of writing,
// leaves a lot to be desired.
TEST_CASE( "200 ints" ) {
    auto x = GENERATE( range( 0,100 ) );
    auto y = GENERATE( range( 200,300 ) );

    CHECK( x < y );
}

#ifdef __cpp_structured_bindings

// One way to do pairs of values (actual/ expected?)
// - the use of structured bindings here is an optional convenience
TEST_CASE( "strlen" ) {
    auto [test_input, expected] = GENERATE( values<std::pair<std::string_view, size_t>>({
            {"one", 3},
            {"two", 3},
            {"three", 5},
            {"four", 4}
        }));

    REQUIRE( test_input.size() == expected );
}

// A nicer way to do pairs (or more) of values - using the table generator.
// Note, you must specify the types up-front
TEST_CASE( "strlen2" ) {
    auto [test_input, expected] = GENERATE( table<std::string, int>({
            {"one", 3},
            {"two", 3},
            {"three", 5},
            {"four", 4}
        }));

    REQUIRE( test_input.size() == expected );
}
#endif

// An alternate way of doing data tables without structure bindings
TEST_CASE( "strlen3" ) {
    struct Data { std::string str; size_t len; };
    auto data = GENERATE( values<Data>({
            {"one", 3},
            {"two", 3},
            {"three", 5},
            {"four", 4}
        }));

    REQUIRE( data.str.size() == data.len );
}


auto square( int i ) -> int { return i*i; }

TEST_CASE( "Random numbers in a range" ) {
    auto x = GENERATE( random( -10000, 10000 ) );
    CAPTURE( x );
    REQUIRE( square(x) >= 0 );
}

#ifdef __cpp_structured_bindings
// Based on example from https://docs.cucumber.io/gherkin/reference/#scenario-outline
// (thanks to https://github.com/catchorg/Catch2/issues/850#issuecomment-399504851)
auto eatCucumbers( int start, int eat ) -> int { return start-eat; }

SCENARIO("Eating cucumbers") {

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
