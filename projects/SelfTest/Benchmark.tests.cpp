#include "catch.hpp"

#include <map>

TEST_CASE( "benchmarked", "[!benchmark]" ) {

    static const int size = 100;

    std::vector<int> v;
    std::map<int, int> m;

    BENCHMARK( "Load up a vector" ) {
        v = std::vector<int>();
        for(int i =0; i < size; ++i )
            v.push_back( i );
    }
    REQUIRE( v.size() == size );

    BENCHMARK( "Load up a map" ) {
        m = std::map<int, int>();
        for(int i =0; i < size; ++i )
            m.insert( { i, i+1 } );
    }
    REQUIRE( m.size() == size );

    BENCHMARK( "Reserved vector" ) {
        v = std::vector<int>();
        v.reserve(size);
        for(int i =0; i < size; ++i )
            v.push_back( i );
    }
    REQUIRE( v.size() == size );

    int array[size];
    BENCHMARK( "A fixed size array that should require no allocations" ) {
        for(int i =0; i < size; ++i )
            array[i] = i;
    }
    int sum = 0;
    for(int i =0; i < size; ++i )
        sum += array[i];
    REQUIRE( sum > size );
}
