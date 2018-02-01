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

// Conversion should go
// StringMaker specialization, operator<<, range/enum detection, unprintable
struct int_iterator {
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = int;
    using reference = int&;
    using pointer = int*;

    int_iterator() = default;
    int_iterator(int i) :val(i) {}

    value_type operator*() const { return val; }
    bool operator==(int_iterator rhs) const { return val == rhs.val; }
    bool operator!=(int_iterator rhs) const { return val != rhs.val; }
    int_iterator operator++() { ++val; return *this; }
    int_iterator operator++(int) {
        auto temp(*this);
        ++val;
        return temp;
    }
private:
    int val = 5;
};

struct streamable_range {
    int_iterator begin() const { return int_iterator{ 1 }; }
    int_iterator end() const { return {}; }
};

std::ostream& operator<<(std::ostream& os, const streamable_range&) {
    os << "op<<(streamable_range)";
    return os;
}

struct stringmaker_range {
    int_iterator begin() const { return int_iterator{ 1 }; }
    int_iterator end() const { return {}; }
};

namespace Catch {
template <>
struct StringMaker<stringmaker_range> {
    static std::string convert(stringmaker_range const&) {
        return "stringmaker(streamable_range)";
    }
};
}

struct just_range {
    int_iterator begin() const { return int_iterator{ 1 }; }
    int_iterator end() const { return {}; }
};

struct disabled_range {
    int_iterator begin() const { return int_iterator{ 1 }; }
    int_iterator end() const { return {}; }
};

namespace Catch {
template <>
struct is_range<disabled_range> {
    static const bool value = false;
};
}

TEST_CASE("toString streamable range", "[toString]") {
    REQUIRE(::Catch::Detail::stringify(streamable_range{}) == "op<<(streamable_range)");
    REQUIRE(::Catch::Detail::stringify(stringmaker_range{}) == "stringmaker(streamable_range)");
    REQUIRE(::Catch::Detail::stringify(just_range{}) == "{ 1, 2, 3, 4 }");
    REQUIRE(::Catch::Detail::stringify(disabled_range{}) == "{?}");
}
