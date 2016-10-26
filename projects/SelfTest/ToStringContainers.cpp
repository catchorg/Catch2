#include "catch.hpp"
#include <vector>

// Containers are only pretty print in c++11
#if defined(CATCH_CPP11_OR_GREATER)
#include <deque>
#include <list>
#include <array>
#include <forward_list>
#endif

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
TEST_CASE( "vector<int> -> toString", "[toString][containers][vector]" ) {
  SequenceTest<std::vector>::integers();
}
TEST_CASE( "vector<string> -> toString", "[toString][containers][vector]" ) {
  SequenceTest<std::vector>::strings();
}
TEST_CASE( "vector<int,allocator> -> toString", "[toString][containers][vector][allocator]" ) {
    SequenceTest<std::vector, MinimalAllocator>::integers();
}
TEST_CASE( "vector<string,allocator> -> toString", "[toString][containers][vector][allocator]" ) {
    SequenceTest<std::vector, MinimalAllocator>::strings();
}
TEST_CASE( "vector<bool> -> toString", "[toString][containers][vector][allocator]" ) {
    std::vector<bool> bools;
    REQUIRE( Catch::toString(bools) == "{  }");
    bools.push_back(true);
    REQUIRE( Catch::toString(bools) == "{ true }");
    bools.push_back(false);
    REQUIRE( Catch::toString(bools) == "{ true, false }");
}

#if defined(CATCH_CPP11_OR_GREATER)

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

// list
TEST_CASE( "list<int> -> toString", "[toString][containers][list]" ) {
  SequenceTest<std::list>::integers();
}
TEST_CASE( "list<string> -> toString", "[toString][containers][list]" ) {
  SequenceTest<std::list>::strings();
}
TEST_CASE( "list<int,allocator> -> toString", "[toString][containers][list][allocator]" ) {
  SequenceTest<std::list, MinimalAllocator>::integers();
}
TEST_CASE( "list<string,allocator> -> toString", "[toString][containers][list][allocator]" ) {
  SequenceTest<std::list, MinimalAllocator>::strings();
}

// deque
TEST_CASE( "deque<int> -> toString", "[toString][containers][deque]" ) {
  SequenceTest<std::deque>::integers();
}
TEST_CASE( "deque<string> -> toString", "[toString][containers][deque]" ) {
  SequenceTest<std::deque>::strings();
}
TEST_CASE( "deque<int,allocator> -> toString", "[toString][containers][deque][allocator]" ) {
    SequenceTest<std::deque, MinimalAllocator>::integers();
}
TEST_CASE( "deque<string,allocator> -> toString", "[toString][containers][deque][allocator]" ) {
    SequenceTest<std::deque, MinimalAllocator>::strings();
}


// C array
TEST_CASE( "int [N] -> toString", "[toString][containers][c-array]" ) {
    // Arrays of size 0 can not exist in c++
    int oneValue[] = { 42 };
    REQUIRE( Catch::toString(oneValue) == "{ 42 }" );
    int twoValues[] = { 42, 250 };
    REQUIRE( Catch::toString(twoValues) == "{ 42, 250 }" );
}

TEST_CASE( "string [N] -> toString", "[toString][containers][c-array]" ) {
    std::string oneValue[] = { "hello" };
    REQUIRE( Catch::toString(oneValue) == "{ \"hello\" }" );
    std::string twoValues[] = { "hello", "world" };
    REQUIRE( Catch::toString(twoValues) == "{ \"hello\", \"world\" }" );
}

TEST_CASE( "char [N] -> toString", "[toString][c-string]" ) {
    // Do not consider char[] as containers but rather as c-string
    char emptyCString[] = "";
    REQUIRE( Catch::toString(emptyCString) == "\"\"" );
    char cstring[] = "hello";
    REQUIRE( Catch::toString(cstring) == "\"hello\"" );
}

// array
TEST_CASE( "array<int, N> -> toString", "[toString][containers][array]" ) {
    std::array<int, 0> empty;
    REQUIRE( Catch::toString(empty) == "{  }" );
    std::array<int, 1> oneValue = { 42 };
    REQUIRE( Catch::toString(oneValue) == "{ 42 }" );
    std::array<int, 2> twoValues = { 42, 250 };
    REQUIRE( Catch::toString(twoValues) == "{ 42, 250 }" );
}

TEST_CASE( "array<string, N> -> toString", "[toString][containers][array]" ) {
    std::array<std::string, 0> empty;
    REQUIRE( Catch::toString(empty) == "{  }" );
    std::array<std::string, 1> oneValue = { "hello" };
    REQUIRE( Catch::toString(oneValue) == "{ \"hello\" }" );
    std::array<std::string, 2> twoValues = { "hello", "world" };
    REQUIRE( Catch::toString(twoValues) == "{ \"hello\", \"world\" }" );
}

/// \brief Specialization for `forward_list` to use
//         `push_front` instead of the unsupported `push_back`.
template <template <class T> class Allocator>
struct SequenceTest<std::forward_list, Allocator> {
    static void integers() {
        std::forward_list<int, Allocator<int> > integers;
        REQUIRE( Catch::toString(integers) == "{  }" );
        integers.push_front( 42 );
        REQUIRE( Catch::toString(integers) == "{ 42 }" );
        integers.push_front( 250 );
        REQUIRE( Catch::toString(integers) == "{ 250, 42 }" );
    };

    static void strings() {
        std::forward_list<std::string, Allocator<std::string> > strings;
        REQUIRE( Catch::toString(strings) == "{  }" );
        strings.push_front( "hello" );
        REQUIRE( Catch::toString(strings) == "{ \"hello\" }" );
        strings.push_front( "world" );
        REQUIRE( Catch::toString(strings) == "{ \"world\", \"hello\" }" );
    }
};

TEST_CASE( "forward_list<int> -> toString", "[toString][containers][forward_list]" ) {
    SequenceTest<std::forward_list>::integers();
}
TEST_CASE( "forward_list<string> -> toString", "[toString][containers][forward_list]" ) {
    SequenceTest<std::forward_list>::strings();
}

TEST_CASE( "forward_list<int,allocator> -> toString", "[toString][containers][forward_list][allocator]" ) {
    SequenceTest<std::forward_list, MinimalAllocator>::integers();
}
TEST_CASE( "forward_list<string,allocator> -> toString", "[toString][containers][forward_list][allocator]" ) {
    SequenceTest<std::forward_list, MinimalAllocator>::strings();
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif
#endif // CATCH_CPP11_OR_GREATER
