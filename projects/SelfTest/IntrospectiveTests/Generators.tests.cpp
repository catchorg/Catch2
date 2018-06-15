
#include "catch.hpp"

#include <iostream> // !DBG
#include <numeric>
#include <map>
#include <set>

#include "internal/catch_generators.hpp"



TEST_CASE("Generators") {
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
    SECTION( "combined" ) {
        auto gen = range( 1, 2 ) << values( { 9, 7 } );

        CHECK( gen.size() == 4 );
        CHECK( gen[0] == 1 );
        CHECK( gen[1] == 2 );
        CHECK( gen[2] == 9 );
        CHECK( gen[3] == 7 );
    }

    SECTION( "values" ) {
        auto gen = Generator<int>() << 3 << 1;

        CHECK( gen.size() == 2 );
        CHECK( gen[0] == 3 );
        CHECK( gen[1] == 1 );
    }

    SECTION( "values first" ) {
        auto gen = 7 << Generator<int>();

        CHECK( gen.size() == 1 );
        CHECK( gen[0] == 7 );
    }

    SECTION( "type erasure" ) {
        auto gen = range( 7, 9 ) << 11;

        // Make type erased version
        auto dynCopy = make_unique<Generator<int>>( std::move( gen ) );
        std::unique_ptr<GeneratorBase const> base = std::move( dynCopy );

        // Only thing we can do is ask for the size
        CHECK( base->size() == 4 );

        // Restore typed version
        auto typed = dynamic_cast<Generator<int> const*>( base.get() );
        REQUIRE( typed );
        CHECK( typed->size() == 4 );
        CHECK( (*typed)[0] == 7 );
        CHECK( (*typed)[3] == 11 );
    }

    SECTION( "memoized" ) {
        GeneratorCache cache;

        std::string id = "test";

        SECTION( "type is deducible" ) {
            using GeneratorType = decltype( range( 11, 14 ) << 27 );
            REQUIRE( std::is_same<GeneratorType, Generator<int>>::value );
            REQUIRE( std::is_same<GeneratorType::type, int>::value );
        }

        SECTION( "macro2" ) {
            int created = 0;
            auto fun = [&]{
                created++;
                return values({42, 7});
            };

            CHECK( generate( cache, id, 0, fun ) == 42 );
            CHECK( created == 1 );
            CHECK( generate( cache, id, 0, fun ) == 42 );
            CHECK( created == 1 );
            CHECK( generate( cache, id, 1, fun ) == 7 );
            CHECK( created == 1 );
        }
    }

    //range( 1, 2 ) << values( { 3.1, 7.9 } ); // should error
//    int i = GENERATE( range(1,3) );

}
