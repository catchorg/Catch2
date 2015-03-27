#include "catch.hpp"
/*
    Demonstrate which version of toString/StringMaker is being used
    for various types
*/


struct has_toString { };
struct has_maker {};
struct has_maker_and_toString {};

namespace Catch {
    inline std::string toString( const has_toString& ) {
        return "toString( has_toString )";
    }
    inline std::string toString( const has_maker_and_toString& ) {
        return "toString( has_maker_and_toString )";
    }
    template<>
    struct StringMaker<has_maker> {
        static std::string convert( const has_maker& ) {
            return "StringMaker<has_maker>";
        }
    };
    template<>
    struct StringMaker<has_maker_and_toString> {
        static std::string convert( const has_maker_and_toString& ) {
            return "StringMaker<has_maker_and_toString>";
        }
    };
}

// Call the overload
TEST_CASE( "toString( has_toString )", "[toString]" ) {
    has_toString item;
    REQUIRE( Catch::toString( item ) == "toString( has_toString )" );
}

// Call the overload
TEST_CASE( "toString( has_maker )", "[toString]" ) {
    has_maker item;
    REQUIRE( Catch::toString( item ) == "StringMaker<has_maker>" );
}

// Call the overload
TEST_CASE( "toString( has_maker_and_toString )", "[toString]" ) {
    has_maker_and_toString item;
    REQUIRE( Catch::toString( item ) == "toString( has_maker_and_toString )" );
}

// Vectors...
TEST_CASE( "toString( vectors<has_toString )", "[toString]" ) {
    std::vector<has_toString> v(1);
    // This invokes template<T> toString which actually gives us '{ ? }'
    REQUIRE( Catch::toString( v ) == "{ {?} }" );
}

TEST_CASE( "toString( vectors<has_maker )", "[toString]" ) {
    std::vector<has_maker> v(1);
    REQUIRE( Catch::toString( v ) == "{ StringMaker<has_maker> }" );
}


TEST_CASE( "toString( vectors<has_maker_and_toString )", "[toString]" ) {
    std::vector<has_maker_and_toString> v(1);
    // Note: This invokes the template<T> toString -> StringMaker
    REQUIRE( Catch::toString( v ) == "{ StringMaker<has_maker_and_toString> }" );
}
