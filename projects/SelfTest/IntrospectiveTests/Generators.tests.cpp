
#include "catch.hpp"

#include <iostream> // !DBG
#include <numeric>
#include <map>
#include <set>

#include "internal/catch_generators.hpp"

namespace Catch {
namespace generators {


} // namespace generators
} // namespace Catch


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
    SECTION( "combined" ) {
        auto gen = range( 1, 2 ) << values( { 9, 7 } );

        CHECK( gen.size() == 4 );
        CHECK( gen[0] == 1 );
        CHECK( gen[1] == 2 );
        CHECK( gen[2] == 9 );
        CHECK( gen[3] == 7 );
    }

    SECTION( "values" ) {
        auto gen = NullGenerator() << 3 << 1;

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

        int created = 0;
        auto fun = [&]{
            created++;
            return values({42, 7});
        };

        // generator is only created on first call
        CHECK( created == 0 );
        CHECK( memoize( cache, id, fun )[0] == 42 );
        CHECK( created == 1 );
        CHECK( memoize( cache, id, fun )[0] == 42 );
        CHECK( created == 1 );
        CHECK( memoize( cache, id, fun )[1] == 7 );
        CHECK( created == 1 );
    }

    SECTION( "strings" ) {
        GeneratorCache cache;
        auto const& gen = memoize( cache, "test", []{ return values({ "one", "two", "three", "four" } ); }  );

        REQUIRE( gen.size() == 4 );
        CHECK( gen[0] == "one" );
        CHECK( gen[1] == "two" );
        CHECK( gen[2] == "three" );
        CHECK( gen[3] == "four" );
    }
}

#define GENERATE( expr ) Catch::generators::generate( CATCH_INTERNAL_LINEINFO, []{ using namespace Catch::generators; return NullGenerator() << expr; } )

TEST_CASE("Generators") {

    auto i = GENERATE( values( { "a", "b", "c" } ) );

    SECTION( "one" ) {
        auto j = GENERATE( range( 8, 11 ) << 2 );
        std::cout << "one: " << i << ", " << j << std::endl;
    }
    SECTION( "two" ) {
        auto j = GENERATE( 3.141 << 1.379 );
        std::cout << "two: " << i << ", " << j << std::endl;
    }
}

TEST_CASE( "200 ints" ) {
    auto x = GENERATE( range( 0,100 ) );
    auto y = GENERATE( range( 200,300 ) );

    CHECK( x < y );
}
