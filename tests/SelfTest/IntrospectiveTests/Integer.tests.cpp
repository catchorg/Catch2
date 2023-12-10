
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_random_integer_helpers.hpp>

namespace {
    template <typename Int>
    static void
    CommutativeMultCheck( Int a, Int b, Int upper_result, Int lower_result ) {
        using Catch::Detail::extendedMult;
        using Catch::Detail::ExtendedMultResult;
        CHECK( extendedMult( a, b ) ==
               ExtendedMultResult<Int>{ upper_result, lower_result } );
        CHECK( extendedMult( b, a ) ==
               ExtendedMultResult<Int>{ upper_result, lower_result } );
    }
} // namespace

TEST_CASE( "extendedMult 64x64", "[Integer][approvals]" ) {
    // a x 0 == 0
    CommutativeMultCheck<uint64_t>( 0x1234'5678'9ABC'DEFF, 0, 0, 0 );

    // bit carried from low half to upper half
    CommutativeMultCheck<uint64_t>( uint64_t( 1 ) << 63, 2, 1, 0 );

    // bits in upper half on one side, bits in lower half on other side
    CommutativeMultCheck<uint64_t>( 0xcdcd'dcdc'0000'0000,
                                    0x0000'0000'aeae'aeae,
                                    0x0000'0000'8c6e'5a77,
                                    0x7391'a588'0000'0000 );

    // Some input numbers without interesting patterns
    CommutativeMultCheck<uint64_t>( 0xaaaa'aaaa'aaaa'aaaa,
                                    0xbbbb'bbbb'bbbb'bbbb,
                                    0x7d27'd27d'27d2'7d26,
                                    0xd82d'82d8'2d82'd82e );

    CommutativeMultCheck<uint64_t>( 0x7d27'd27d'27d2'7d26,
                                    0xd82d'82d8'2d82'd82e,
                                    0x69af'd991'8256'b953,
                                    0x8724'8909'fcb6'8cd4 );

    CommutativeMultCheck<uint64_t>( 0xdead'beef'dead'beef,
                                    0xfeed'feed'feed'feef,
                                    0xddbf'680b'2b0c'b558,
                                    0x7a36'b06f'2ce9'6321 );

    CommutativeMultCheck<uint64_t>( 0xddbf'680b'2b0c'b558,
                                    0x7a36'b06f'2ce9'6321,
                                    0x69dc'96c9'294b'fc7f,
                                    0xd038'39fa'a3dc'6858 );

    CommutativeMultCheck<uint64_t>( 0x61c8'8646'80b5'83eb,
                                    0x61c8'8646'80b5'83eb,
                                    0x2559'92d3'8220'8bbe,
                                    0xdf44'2d22'ce48'59b9 );
}

TEST_CASE( "SizedUnsignedType helpers", "[integer][approvals]" ) {
    using Catch::Detail::SizedUnsignedType_t;
    using Catch::Detail::DoubleWidthUnsignedType_t;

    STATIC_REQUIRE( sizeof( SizedUnsignedType_t<1> ) == 1 );
    STATIC_REQUIRE( sizeof( SizedUnsignedType_t<2> ) == 2 );
    STATIC_REQUIRE( sizeof( SizedUnsignedType_t<4> ) == 4 );
    STATIC_REQUIRE( sizeof( SizedUnsignedType_t<8> ) == 8 );

    STATIC_REQUIRE( sizeof( DoubleWidthUnsignedType_t<std::uint8_t> ) == 2 );
    STATIC_REQUIRE( std::is_unsigned<DoubleWidthUnsignedType_t<std::uint8_t>>::value );
    STATIC_REQUIRE( sizeof( DoubleWidthUnsignedType_t<std::uint16_t> ) == 4 );
    STATIC_REQUIRE( std::is_unsigned<DoubleWidthUnsignedType_t<std::uint16_t>>::value );
    STATIC_REQUIRE( sizeof( DoubleWidthUnsignedType_t<std::uint32_t> ) == 8 );
    STATIC_REQUIRE( std::is_unsigned<DoubleWidthUnsignedType_t<std::uint32_t>>::value );
}

TEST_CASE( "extendedMult 32x32", "[integer][approvals]" ) {
    // a x 0 == 0
    CommutativeMultCheck<uint32_t>( 0x1234'5678, 0, 0, 0 );

    // bit carried from low half to upper half
    CommutativeMultCheck<uint32_t>( uint32_t(1) << 31, 2, 1, 0 );

    // bits in upper half on one side, bits in lower half on other side
    CommutativeMultCheck<uint32_t>( 0xdcdc'0000, 0x0000'aabb, 0x0000'934b, 0x6cb4'0000 );

    // Some input numbers without interesting patterns
    CommutativeMultCheck<uint32_t>(
        0xaaaa'aaaa, 0xbbbb'bbbb, 0x7d27'd27c, 0x2d82'd82e );

    CommutativeMultCheck<uint32_t>(
        0x7d27'd27c, 0x2d82'd82e, 0x163f'f7e8, 0xc5b8'7248 );

    CommutativeMultCheck<uint32_t>(
        0xdead'beef, 0xfeed'feed, 0xddbf'6809, 0x6f8d'e543 );

    CommutativeMultCheck<uint32_t>(
        0xddbf'6809, 0x6f8d'e543, 0x60a0'e71e, 0x751d'475b );
}

TEST_CASE( "extendedMult 8x8", "[integer][approvals]" ) {
    // a x 0 == 0
    CommutativeMultCheck<uint8_t>( 0xcd, 0, 0, 0 );

    // bit carried from low half to upper half
    CommutativeMultCheck<uint8_t>( uint8_t( 1 ) << 7, 2, 1, 0 );

    // bits in upper half on one side, bits in lower half on other side
    CommutativeMultCheck<uint8_t>( 0x80, 0x03, 0x01, 0x80 );

    // Some input numbers without interesting patterns
    CommutativeMultCheck<uint8_t>( 0xaa, 0xbb, 0x7c, 0x2e );
    CommutativeMultCheck<uint8_t>( 0x7c, 0x2e, 0x16, 0x48 );
    CommutativeMultCheck<uint8_t>( 0xdc, 0xcd, 0xb0, 0x2c );
    CommutativeMultCheck<uint8_t>( 0xb0, 0x2c, 0x1e, 0x40 );
}


TEST_CASE( "negative and positive signed integers keep their order after transposeToNaturalOrder",
                    "[integer][approvals]") {
    using Catch::Detail::transposeToNaturalOrder;
    int32_t negative( -1 );
    int32_t positive( 1 );
    uint32_t adjusted_negative =
        transposeToNaturalOrder<int32_t>( static_cast<uint32_t>( negative ) );
    uint32_t adjusted_positive =
        transposeToNaturalOrder<int32_t>( static_cast<uint32_t>( positive ) );
    REQUIRE( adjusted_negative < adjusted_positive );
    REQUIRE( adjusted_positive - adjusted_negative == 2 );

    // Conversion has to be reversible
    REQUIRE( negative == static_cast<int32_t>( transposeToNaturalOrder<int32_t>(
                             adjusted_negative ) ) );
    REQUIRE( positive == static_cast<int32_t>( transposeToNaturalOrder<int32_t>(
                             adjusted_positive ) ) );
}

TEST_CASE( "unsigned integers are unchanged by transposeToNaturalOrder",
           "[integer][approvals]") {
    using Catch::Detail::transposeToNaturalOrder;
    uint32_t max = std::numeric_limits<uint32_t>::max();
    uint32_t zero = 0;
    REQUIRE( max == transposeToNaturalOrder<uint32_t>( max ) );
    REQUIRE( zero == transposeToNaturalOrder<uint32_t>( zero ) );
}
