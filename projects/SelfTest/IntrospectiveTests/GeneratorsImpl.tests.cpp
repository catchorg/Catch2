#include "catch.hpp"

// Tests of generartor implementation details

TEST_CASE("Generators impl", "[impl]") {
    using namespace Catch::Generators;

    SECTION( "range" ) {
        auto gen = range(1,3);

        CHECK( gen.size() == 2 );

        CHECK( gen[0] == 1 );
        CHECK( gen[1] == 2 );
    }
    SECTION( "fixed values" ) {
        auto gen = values( { 3, 1, 4, 1 } );

        CHECK( gen.size() == 4 );
        CHECK( gen[0] == 3 );
        CHECK( gen[1] == 1 );
        CHECK( gen[2] == 4 );
        CHECK( gen[3] == 1 );
    }
    SECTION( "combined" ) {
        auto gen = makeGenerators( range( 1, 3 ), values( { 9, 7 } ) );

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
        auto gen = makeGenerators( range( 7, 10 ), 11 );

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

TEST_CASE("Generators impl - random", "[approvals]") {
    using namespace Catch::Generators;

    SECTION( "random range" ) {
        auto gen = random( 3, 9 );

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
}
