#define CATCH_CONFIG_ENABLE_VARIANT_STRINGMAKER
#include "catch.hpp"

#if defined(CATCH_CONFIG_CPP17_VARIANT)

#include <string>
#include <variant>

TEST_CASE( "variant<std::monostate>", "[toString][variant][approvals]")
{
    using type = std::variant<std::monostate>;
    CHECK( "{ }" == ::Catch::Detail::stringify(type{}) );
    type value {};
    CHECK( "{ }" == ::Catch::Detail::stringify(value) );
    CHECK( "{ }" == ::Catch::Detail::stringify(std::get<0>(value)) );
}

TEST_CASE( "variant<int>", "[toString][variant][approvals]")
{
    using type = std::variant<int>;
    CHECK( "0" == ::Catch::Detail::stringify(type{0}) );
}

TEST_CASE( "variant<float, int>", "[toString][variant][approvals]")
{
    using type = std::variant<float, int>;
    CHECK( "0.5f" == ::Catch::Detail::stringify(type{0.5f}) );
    CHECK( "0" == ::Catch::Detail::stringify(type{0}) );

    SECTION("valueless by exception") {
        struct sample {
            operator int() const { throw 42; }
        };

        type value{1.5f};
        REQUIRE_THROWS_AS( value.emplace<int>(sample{}), int );
        REQUIRE( value.valueless_by_exception() );
        CHECK( "{valueless variant}" == ::Catch::Detail::stringify(value) );
    }
}

TEST_CASE( "variant<string, int>", "[toString][variant][approvals]")
{
    using type = std::variant<std::string, int>;
    CHECK( "\"foo\"" == ::Catch::Detail::stringify(type{"foo"}) );
    CHECK( "0" == ::Catch::Detail::stringify(type{0}) );
}

TEST_CASE( "variant<variant<float, int>, string>", "[toString][variant][approvals]")
{
    using inner = std::variant<float, int>;
    using type = std::variant<inner, std::string>;
    CHECK( "0.5f" == ::Catch::Detail::stringify(type{0.5f}) );
    CHECK( "0" == ::Catch::Detail::stringify(type{0}) );
    CHECK( "\"foo\"" == ::Catch::Detail::stringify(type{"foo"}) );

    struct sample {
        operator int() const { throw 42; }
    };

    SECTION("valueless nested variant") {
        type value = inner{0.5f};
        REQUIRE( std::holds_alternative<inner>(value) );
        REQUIRE( std::holds_alternative<float>(std::get<inner>(value)) );

        REQUIRE_THROWS_AS( std::get<0>(value).emplace<int>(sample{}), int );

        // outer variant is still valid and contains inner
        REQUIRE( std::holds_alternative<inner>(value) );
        // inner variant is valueless
        REQUIRE( std::get<inner>(value).valueless_by_exception() );
        CHECK( "{valueless variant}" == ::Catch::Detail::stringify(value) );
    }
}

TEST_CASE( "variant<nullptr,int,const char *>", "[toString][variant][approvals]" )
{
    using type = std::variant<std::nullptr_t,int,const char *>;
    CHECK( "nullptr" == ::Catch::Detail::stringify(type{nullptr}) );
    CHECK( "42" == ::Catch::Detail::stringify(type{42}) );
    CHECK( "\"Catch me\"" == ::Catch::Detail::stringify(type{"Catch me"}) );
}

#endif // CATCH_INTERNAL_CONFIG_CPP17_VARIANT
