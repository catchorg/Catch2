#include "catch.hpp"


namespace {
// Enum without user-provided stream operator
enum Enum1 { Enum1Value0, Enum1Value1 };

// Enum with user-provided stream operator
enum Enum2 { Enum2Value0, Enum2Value1 };

std::ostream& operator<<( std::ostream& os, Enum2 v ) {
    return os << "E2{" << static_cast<int>(v) << "}";
}
} // end anonymous namespace

TEST_CASE( "toString(enum)", "[toString][enum]" ) {
    Enum1 e0 = Enum1Value0;
    CHECK( ::Catch::Detail::stringify(e0) == "0" );
    Enum1 e1 = Enum1Value1;
    CHECK( ::Catch::Detail::stringify(e1) == "1" );
}

TEST_CASE( "toString(enum w/operator<<)", "[toString][enum]" ) {
    Enum2 e0 = Enum2Value0;
    CHECK( ::Catch::Detail::stringify(e0) == "E2{0}" );
    Enum2 e1 = Enum2Value1;
    CHECK( ::Catch::Detail::stringify(e1) == "E2{1}" );
}

// Enum class without user-provided stream operator
namespace {
enum class EnumClass1 { EnumClass1Value0, EnumClass1Value1 };

// Enum class with user-provided stream operator
enum class EnumClass2 { EnumClass2Value0, EnumClass2Value1 };

std::ostream& operator<<( std::ostream& os, EnumClass2 e2 ) {
    switch( static_cast<int>( e2 ) ) {
        case static_cast<int>( EnumClass2::EnumClass2Value0 ):
            return os << "E2/V0";
        case static_cast<int>( EnumClass2::EnumClass2Value1 ):
            return os << "E2/V1";
        default:
            return os << "Unknown enum value " << static_cast<int>( e2 );
    }
}

} // end anonymous namespace

TEST_CASE( "toString(enum class)", "[toString][enum][enumClass]" ) {
    EnumClass1 e0 = EnumClass1::EnumClass1Value0;
    CHECK( ::Catch::Detail::stringify(e0) == "0" );
    EnumClass1 e1 = EnumClass1::EnumClass1Value1;
    CHECK( ::Catch::Detail::stringify(e1) == "1" );
}


TEST_CASE( "toString(enum class w/operator<<)", "[toString][enum][enumClass]" ) {
    EnumClass2 e0 = EnumClass2::EnumClass2Value0;
    CHECK( ::Catch::Detail::stringify(e0) == "E2/V0" );
    EnumClass2 e1 = EnumClass2::EnumClass2Value1;
    CHECK( ::Catch::Detail::stringify(e1) == "E2/V1" );

    EnumClass2 e3 = static_cast<EnumClass2>(10);
    CHECK( ::Catch::Detail::stringify(e3) == "Unknown enum value 10" );
}

enum class EnumClass3 { Value1, Value2, Value3, Value4 };

REGISTER_ENUM( EnumClass3, EnumClass3::Value1, EnumClass3::Value2, EnumClass3::Value3 )


TEST_CASE( "REGISTER_ENUM" ) {
    using Catch::Detail::stringify;
    REQUIRE( stringify( EnumClass3::Value1 ) == "Value1" );
    REQUIRE( stringify( EnumClass3::Value2 ) == "Value2" );
    REQUIRE( stringify( EnumClass3::Value3 ) == "Value3" );
    REQUIRE( stringify( EnumClass3::Value4 ) == "{** unexpected enum value **}" );

    EnumClass3 ec3 = EnumClass3 ::Value2;
    REQUIRE( stringify( ec3 ) == "Value2" );
}

#include "internal/catch_interfaces_enum_values_registry.h"

TEST_CASE( "EnumInfo" ) {

    auto& hub = Catch::getMutableRegistryHub();
    auto& reg = hub.getMutableEnumValuesRegistry();
    auto const& enumInfo = reg.registerEnum( "EnumName", "EnumName::Value1, EnumName::Value2", {0, 1} );

    CHECK( enumInfo.lookup(0) == "Value1" );
    CHECK( enumInfo.lookup(1) == "Value2" );
    CHECK( enumInfo.lookup(3) == "{** unexpected enum value **}" );
}

#include "internal/catch_enum_values_registry.h"

TEST_CASE( "parseEnums", "[Strings][enums]" ) {
    using namespace Catch::Matchers;
    using Catch::Detail::parseEnums;

    SECTION( "No enums" )
        CHECK_THAT( parseEnums( "" ), Equals( std::vector<std::string>{} ) );

    SECTION( "One enum value" ) {
        CHECK_THAT( parseEnums( "ClassName::EnumName::Value1" ), Equals(std::vector<std::string>{"Value1"} ) );
        CHECK_THAT( parseEnums( "Value1" ), Equals( std::vector<std::string>{"Value1"} ) );
        CHECK_THAT( parseEnums( "EnumName::Value1" ), Equals(std::vector<std::string>{"Value1"} ) );
    }

    SECTION( "Multiple enum values" ) {
        CHECK_THAT( parseEnums( "ClassName::EnumName::Value1, ClassName::EnumName::Value2" ),
                    Equals( std::vector<std::string>{"Value1", "Value2"} ) );
        CHECK_THAT( parseEnums( "ClassName::EnumName::Value1, ClassName::EnumName::Value2, ClassName::EnumName::Value3" ),
                    Equals( std::vector<std::string>{"Value1", "Value2", "Value3"} ) );
        CHECK_THAT( parseEnums( "ClassName::EnumName::Value1,ClassName::EnumName::Value2 , ClassName::EnumName::Value3" ),
                    Equals( std::vector<std::string>{"Value1", "Value2", "Value3"} ) );
    }
}
