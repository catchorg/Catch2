#include <catch2/internal/catch_enum_values_registry.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_test_macros.hpp>

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