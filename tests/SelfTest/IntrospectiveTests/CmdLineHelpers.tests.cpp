
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_reporter_spec_parser.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/interfaces/catch_interfaces_config.hpp>

TEST_CASE("Reporter spec splitting", "[reporter-spec][cli][approvals]") {
	using Catch::Detail::splitReporterSpec;
	using Catch::Matchers::Equals;
	using namespace std::string_literals;

    SECTION("Various edge cases") {
        REQUIRE_THAT( splitReporterSpec( "" ),
                      Equals( std::vector<std::string>{ ""s } ) );
        REQUIRE_THAT( splitReporterSpec( "::" ),
                      Equals( std::vector<std::string>{ "", "" } ) );
        REQUIRE_THAT( splitReporterSpec( "::rep" ),
                      Equals( std::vector<std::string>{ "", "rep" } ) );
        REQUIRE_THAT( splitReporterSpec( "rep::" ),
                      Equals( std::vector<std::string>{ "rep", "" } ) );

    }

    SECTION("Validish specs") {
        REQUIRE_THAT( splitReporterSpec( "newReporter" ),
                      Equals( std::vector<std::string>{ "newReporter"s } ) );
        REQUIRE_THAT(
            splitReporterSpec( "foo-reporter::key1=value1::key2=value with "
                               "space::key with space=some-value" ),
            Equals(
                std::vector<std::string>{ "foo-reporter"s,
                                          "key1=value1"s,
                                          "key2=value with space"s,
                                          "key with space=some-value"s } ) );
        REQUIRE_THAT(
            splitReporterSpec( "spaced reporter name::key:key=value:value" ),
            Equals( std::vector<std::string>{ "spaced reporter name"s,
                                              "key:key=value:value"s } ) );
    }
}

TEST_CASE( "Parsing colour mode", "[cli][colour][approvals]" ) {
    using Catch::Detail::stringToColourMode;
    using Catch::ColourMode;
    SECTION("Valid strings") {
        REQUIRE( stringToColourMode( "none" ) == ColourMode::None );
        REQUIRE( stringToColourMode( "ansi" ) == ColourMode::ANSI );
        REQUIRE( stringToColourMode( "win32" ) == ColourMode::Win32 );
        REQUIRE( stringToColourMode( "default" ) ==
                 ColourMode::PlatformDefault );
    }
    SECTION("Wrong strings") {
        REQUIRE_FALSE( stringToColourMode( "NONE" ) );
        REQUIRE_FALSE( stringToColourMode( "-" ) );
        REQUIRE_FALSE( stringToColourMode( "asdbjsdb kasbd" ) );
    }
}
