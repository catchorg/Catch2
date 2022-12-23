
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/internal/catch_floating_point_helpers.hpp>


TEST_CASE("convertToBits", "[floating-point][conversion]") {
    using Catch::Detail::convertToBits;

    CHECK( convertToBits( 0.f ) == 0 );
    CHECK( convertToBits( -0.f ) == ( 1ULL << 31 ) );
    CHECK( convertToBits( 0. ) == 0 );
    CHECK( convertToBits( -0. ) == ( 1ULL << 63 ) );
    CHECK( convertToBits( std::numeric_limits<float>::denorm_min() ) == 1 );
    CHECK( convertToBits( std::numeric_limits<double>::denorm_min() ) == 1 );
}

TEMPLATE_TEST_CASE("type-shared ulpDistance tests", "[floating-point][ulp][approvals]", float, double) {
    using FP = TestType;
    using Catch::ulpDistance;

    // Distance between zeros is zero
    CHECK( ulpDistance( FP{}, FP{} ) == 0 );
    CHECK( ulpDistance( FP{}, -FP{} ) == 0 );
    CHECK( ulpDistance( -FP{}, -FP{} ) == 0 );

    // Distance between same-sign infinities is zero
    static constexpr FP infinity = std::numeric_limits<FP>::infinity();
    CHECK( ulpDistance( infinity, infinity ) == 0 );
    CHECK( ulpDistance( -infinity, -infinity ) == 0 );

    // Distance between max-finite-val and same sign infinity is 1
    static constexpr FP max_finite = std::numeric_limits<FP>::max();
    CHECK( ulpDistance( max_finite, infinity ) == 1 );
    CHECK( ulpDistance( -max_finite, -infinity ) == 1 );

    // Distance between X and 0 is half of distance between X and -X
    CHECK( ulpDistance( -infinity, infinity ) ==
           2 * ulpDistance( infinity, FP{} ) );
    CHECK( 2 * ulpDistance( FP{ -2. }, FP{} ) ==
           ulpDistance( FP{ -2. }, FP{ 2. } ) );
    CHECK( 2 * ulpDistance( FP{ 2. }, FP{} ) ==
           ulpDistance( FP{ -2. }, FP{ 2. } ) );

    // Denorms are supported
    CHECK( ulpDistance( std::numeric_limits<FP>::denorm_min(), FP{} ) == 1 );
    CHECK( ulpDistance( std::numeric_limits<FP>::denorm_min(), -FP{} ) == 1 );
    CHECK( ulpDistance( -std::numeric_limits<FP>::denorm_min(), FP{} ) == 1 );
    CHECK( ulpDistance( -std::numeric_limits<FP>::denorm_min(), -FP{} ) == 1 );
    CHECK( ulpDistance( std::numeric_limits<FP>::denorm_min(),
                        -std::numeric_limits<FP>::denorm_min() ) == 2 );

    // Machine epsilon
    CHECK( ulpDistance( FP{ 1. },
                        FP{ 1. } + std::numeric_limits<FP>::epsilon() ) == 1 );
    CHECK( ulpDistance( -FP{ 1. },
                        -FP{ 1. } - std::numeric_limits<FP>::epsilon() ) == 1 );
}

TEST_CASE("UlpDistance", "[floating-point][ulp][approvals]") {
    using Catch::ulpDistance;

    CHECK( ulpDistance( 1., 2. ) == 0x10'00'00'00'00'00'00 );
    CHECK( ulpDistance( -2., 2. ) == 0x80'00'00'00'00'00'00'00 );
    CHECK( ulpDistance( 1.f, 2.f ) == 0x80'00'00 );
    CHECK( ulpDistance( -2.f, 2.f ) == 0x80'00'00'00 );
}
