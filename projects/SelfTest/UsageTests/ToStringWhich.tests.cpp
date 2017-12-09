#include "catch.hpp"
/*
    Demonstrate which version of toString/StringMaker is being used
    for various types
*/


struct has_operator { };
struct has_maker {};
struct has_maker_and_operator {};

std::ostream& operator<<(std::ostream& os, const has_operator&) {
    os << "operator<<( has_operator )";
    return os;
}

std::ostream& operator<<(std::ostream& os, const has_maker_and_operator&) {
    os << "operator<<( has_maker_and_operator )";
    return os;
}

namespace Catch {
    template<>
    struct StringMaker<has_maker> {
        static std::string convert( const has_maker& ) {
            return "StringMaker<has_maker>";
        }
    };
    template<>
    struct StringMaker<has_maker_and_operator> {
        static std::string convert( const has_maker_and_operator& ) {
            return "StringMaker<has_maker_and_operator>";
        }
    };
}

// Call the operator
TEST_CASE( "stringify( has_operator )", "[toString]" ) {
    has_operator item;
    REQUIRE( ::Catch::Detail::stringify( item ) == "operator<<( has_operator )" );
}

// Call the stringmaker
TEST_CASE( "stringify( has_maker )", "[toString]" ) {
    has_maker item;
    REQUIRE( ::Catch::Detail::stringify( item ) == "StringMaker<has_maker>" );
}

// Call the stringmaker
TEST_CASE( "stringify( has_maker_and_toString )", "[.][toString]" ) {
    has_maker_and_operator item;
    REQUIRE( ::Catch::Detail::stringify( item ) == "StringMaker<has_maker_and_operator>" );
}

// Vectors...

// Don't run this in approval tests as it is sensitive to two phase lookup differences
TEST_CASE( "toString( vectors<has_operator> )", "[toString]" ) {
    std::vector<has_operator> v(1);
    REQUIRE( ::Catch::Detail::stringify( v ) == "{ operator<<( has_operator ) }" );
}

TEST_CASE( "toString( vectors<has_maker> )", "[toString]" ) {
    std::vector<has_maker> v(1);
    REQUIRE( ::Catch::Detail::stringify( v ) == "{ StringMaker<has_maker> }" );
}


// Don't run this in approval tests as it is sensitive to two phase lookup differences
TEST_CASE( "toString( vectors<has_maker_and_operator> )", "[toString]" ) {
    std::vector<has_maker_and_operator> v(1);
    REQUIRE( ::Catch::Detail::stringify( v ) == "{ StringMaker<has_maker_and_operator> }" );
}
