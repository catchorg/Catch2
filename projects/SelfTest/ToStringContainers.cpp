#include "catch.hpp"
#include <vector>
#include <deque>
#include <list>

/// \file Test Catch::to_string for standard containors.

/// \brief Test for sequence containors
/// \tparm Sequence The containor to test.
/// \tparm Allocator The containor element allocator to use.
template <template <class T, class Allocator = std::allocator<T> > class Sequence,
          template <class T> class Allocator = std::allocator>
struct SequenceTest {
    static void integers() {
        Sequence<int, Allocator<int> > integers;
        REQUIRE( Catch::toString(integers) == "{  }" );
        integers.push_back( 42 );
        REQUIRE( Catch::toString(integers) == "{ 42 }" );
        integers.push_back( 250 );
        REQUIRE( Catch::toString(integers) == "{ 42, 250 }" );
    };

    static void strings() {
        Sequence<std::string, Allocator<std::string> > strings;
        REQUIRE( Catch::toString(strings) == "{  }" );
        strings.push_back( "hello" );
        REQUIRE( Catch::toString(strings) == "{ \"hello\" }" );
        strings.push_back( "world" );
        REQUIRE( Catch::toString(strings) == "{ \"hello\", \"world\" }" );
    }
};

// vector
TEST_CASE( "vector<int> -> toString", "[toString][containers][vector]" ) {
    SequenceTest<std::vector>::integers();
}
TEST_CASE( "vector<string> -> toString", "[toString][containers][vector]" ) {
    SequenceTest<std::vector>::strings();
}

namespace {
    /** \brief Custom allocator, should not impact toString. */
    template<typename T>
    struct MinimalAllocator : std::allocator<T> {
        typedef typename std::allocator<T>::size_type size_type;
        T *allocate( size_type n ) {
            return static_cast<T *>( ::operator new( n * sizeof(T) ) );
        }
        void deallocate( T *p, size_type /*n*/ ) {
            ::operator delete( static_cast<void *>(p) );
        }
    };
}

// vector
TEST_CASE( "vector<int,allocator> -> toString", "[toString][containers][vector][allocator]" ) {
    SequenceTest<std::vector, MinimalAllocator>::integers();
}
TEST_CASE( "vector<string,allocator> -> toString", "[toString][containers][vector][allocator]" ) {
    SequenceTest<std::vector, MinimalAllocator>::strings();
}
