
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/internal/catch_enum_values_registry.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

enum class EnumClass3 { Value1, Value2, Value3, Value4 };

struct UsesSentinel {
    using const_iterator = int const*;
    using const_sentinel = std::nullptr_t;

    const_iterator begin() const { return nullptr; }
    const_iterator end() const { return nullptr; }
};

TEST_CASE( "parseEnums", "[Strings][enums]" ) {
    using namespace Catch::Matchers;
    using Catch::Detail::parseEnums;

    SECTION( "No enums" )
        CHECK_THAT( parseEnums( "" ), Equals( std::vector<Catch::StringRef>{} ) );

    SECTION( "One enum value" ) {
        CHECK_THAT( parseEnums( "ClassName::EnumName::Value1" ),
                Equals(std::vector<Catch::StringRef>{"Value1"} ) );
        CHECK_THAT( parseEnums( "Value1" ),
                Equals( std::vector<Catch::StringRef>{"Value1"} ) );
        CHECK_THAT( parseEnums( "EnumName::Value1" ),
                Equals(std::vector<Catch::StringRef>{"Value1"} ) );
    }

    SECTION( "Multiple enum values" ) {
        CHECK_THAT( parseEnums( "ClassName::EnumName::Value1, ClassName::EnumName::Value2" ),
                    Equals( std::vector<Catch::StringRef>{"Value1", "Value2"} ) );
        CHECK_THAT( parseEnums( "ClassName::EnumName::Value1, ClassName::EnumName::Value2, ClassName::EnumName::Value3" ),
                    Equals( std::vector<Catch::StringRef>{"Value1", "Value2", "Value3"} ) );
        CHECK_THAT( parseEnums( "ClassName::EnumName::Value1,ClassName::EnumName::Value2 , ClassName::EnumName::Value3" ),
                    Equals( std::vector<Catch::StringRef>{"Value1", "Value2", "Value3"} ) );
    }
}

TEST_CASE( "Directly creating an EnumInfo" ) {

    using namespace Catch::Detail;
    auto enumInfo = makeEnumInfo( "EnumName", "EnumName::Value1, EnumName::Value2", {0, 1} );

    CHECK( enumInfo->lookup(0) == "Value1" );
    CHECK( enumInfo->lookup(1) == "Value2" );
    CHECK( enumInfo->lookup(3) == "{** unexpected enum value **}" );
}

TEST_CASE("Range type with sentinel") {
    CHECK( Catch::Detail::stringify(UsesSentinel{}) == "{  }" );
}

TEST_CASE("convertIntoString stringification helper", "[toString][approvals]") {
    using namespace std::string_literals;
    using Catch::Detail::convertIntoString;
    using namespace Catch;

    SECTION("No escaping") {
        CHECK(convertIntoString(""_sr, false) == R"("")"s);
        CHECK(convertIntoString("abcd"_sr, false) == R"("abcd")"s);
        CHECK(convertIntoString("ab\ncd"_sr, false) == "\"ab\ncd\""s);
        CHECK(convertIntoString("ab\r\ncd"_sr, false) == "\"ab\r\ncd\""s);
        CHECK(convertIntoString("ab\"cd"_sr, false) == R"("ab"cd")"s);
    }
    SECTION("Escaping invisibles") {
        CHECK(convertIntoString(""_sr, true) == R"("")"s);
        CHECK(convertIntoString("ab\ncd"_sr, true) == R"("ab\ncd")"s);
        CHECK(convertIntoString("ab\r\ncd"_sr, true) == R"("ab\r\ncd")"s);
        CHECK(convertIntoString("ab\tcd"_sr, true) == R"("ab\tcd")"s);
        CHECK(convertIntoString("ab\fcd"_sr, true) == R"("ab\fcd")"s);
        CHECK(convertIntoString("ab\"cd"_sr, true) == R"("ab"cd")"s);
    }
}

TEMPLATE_TEST_CASE( "Stringifying char arrays with statically known sizes",
                    "[toString]",
                    char,
                    signed char,
                    unsigned char ) {
    using namespace std::string_literals;
    TestType with_null_terminator[10] = "abc";
    CHECK( ::Catch::Detail::stringify( with_null_terminator ) == R"("abc")"s );

    TestType no_null_terminator[3] = { 'a', 'b', 'c' };
    CHECK( ::Catch::Detail::stringify( no_null_terminator ) == R"("abc")"s );
}
