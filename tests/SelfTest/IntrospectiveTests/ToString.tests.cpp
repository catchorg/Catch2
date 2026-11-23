
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <chrono>

struct UsesSentinel {
    using const_iterator = int const*;
    using const_sentinel = std::nullptr_t;

    const_iterator begin() const { return nullptr; }
    const_sentinel end() const { return nullptr; }
};

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

TEST_CASE( "#2944 - Stringifying dates before 1970 should not crash", "[.approvals]" ) {
    using Catch::Matchers::Equals;
    using Days = std::chrono::duration<int32_t, std::ratio<86400>>;
    using SysDays = std::chrono::time_point<std::chrono::system_clock, Days>;
    using SM = Catch::StringMaker<std::chrono::system_clock::time_point>;

    // Check simple date first
    const SysDays post1970{ Days{ 1 } };
    auto converted_post = SM::convert( post1970 );
    REQUIRE( converted_post == "1970-01-02T00:00:00Z" );

    const SysDays pre1970{ Days{ -1 } };
    auto converted_pre = SM::convert( pre1970 );
    REQUIRE_THAT(
        converted_pre,
        Equals( "1969-12-31T00:00:00Z" ) ||
            Equals( "gmtime from provided timepoint has failed. This "
                    "happens e.g. with pre-1970 dates using Microsoft libc" ) );
}

namespace {
    struct ThrowsOnStringification {
        friend bool operator==( ThrowsOnStringification,
                                ThrowsOnStringification ) {
            return true;
        }
    };
}

template <>
struct Catch::StringMaker<ThrowsOnStringification> {
    static std::string convert(ThrowsOnStringification) {
        throw std::runtime_error( "Invalid" );
    }
};

TEST_CASE( "Exception thrown inside stringify does not fail the test", "[toString]" ) {
    ThrowsOnStringification tos;
    CHECK( tos == tos );
}

TEST_CASE( "string escaping benchmark", "[toString][!benchmark]" ) {
    const auto input_length = GENERATE( as<size_t>{}, 10, 100, 10'000, 100'000 );
    std::string test_input( input_length, 'a' );
    BENCHMARK( "no-escape string, no-escaping, len=" +
               std::to_string( input_length ) ) {
        return Catch::Detail::convertIntoString( test_input, false );
    };
    BENCHMARK( "no-escape string, escaping, len=" +
               std::to_string( input_length ) ) {
        return Catch::Detail::convertIntoString( test_input, true );
    };

    std::string escape_input( input_length, '\r' );
    BENCHMARK( "full escape string, no-escaping, len=" +
               std::to_string( input_length ) ) {
        return Catch::Detail::convertIntoString( escape_input, false );
    };
    BENCHMARK( "full escape string, escaping, len=" +
               std::to_string( input_length ) ) {
        return Catch::Detail::convertIntoString( escape_input, true );
    };
}
