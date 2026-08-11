
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_generator_filter_parser.hpp>

TEST_CASE( "parseGeneratorIndexSpec", "[generators][cli]" ) {
    using Catch::GeneratorFilterParseResult;
    using Catch::parseGeneratorIndexSpec;

    SECTION( "single index" ) {
        auto const result = parseGeneratorIndexSpec( "42" );
        REQUIRE( result.type == GeneratorFilterParseResult::Type::Ok );
        REQUIRE( result.indices == std::vector<unsigned int>{ 42 } );
    }

    SECTION( "comma-separated indices" ) {
        auto const result = parseGeneratorIndexSpec( "1,3,4" );
        REQUIRE( result.type == GeneratorFilterParseResult::Type::Ok );
        REQUIRE( result.indices == ( std::vector<unsigned int>{ 1, 3, 4 } ) );
    }

    SECTION( "closed range" ) {
        auto const result = parseGeneratorIndexSpec( "1-3" );
        REQUIRE( result.type == GeneratorFilterParseResult::Type::Ok );
        REQUIRE( result.indices == ( std::vector<unsigned int>{ 1, 2, 3 } ) );
    }

    SECTION( "combined list and ranges" ) {
        auto const result = parseGeneratorIndexSpec( "1,3,6-10" );
        REQUIRE( result.type == GeneratorFilterParseResult::Type::Ok );
        REQUIRE( result.indices ==
                 ( std::vector<unsigned int>{ 1, 3, 6, 7, 8, 9, 10 } ) );
    }

    SECTION( "rejects overlapping ranges" ) {
        auto const result = parseGeneratorIndexSpec( "1-3,2-4" );
        REQUIRE( result.type == GeneratorFilterParseResult::Type::Error );
    }
}
