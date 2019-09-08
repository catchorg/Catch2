#include "catch.hpp"

#include "internal/catch_enum_values_registry.h"

enum class EnumClass3 { Value1, Value2, Value3, Value4 };


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
    std::unique_ptr<EnumInfo> enumInfo = makeEnumInfo( "EnumName", "EnumName::Value1, EnumName::Value2", {0, 1} );

    CHECK( enumInfo->lookup(0) == "Value1" );
    CHECK( enumInfo->lookup(1) == "Value2" );
    CHECK( enumInfo->lookup(3) == "{** unexpected enum value **}" );
}
