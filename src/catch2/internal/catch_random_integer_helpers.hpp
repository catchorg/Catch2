
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#ifndef CATCH_RANDOM_INTEGER_HELPERS_HPP_INCLUDED
#define CATCH_RANDOM_INTEGER_HELPERS_HPP_INCLUDED

#include <climits>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Catch {
    namespace Detail {

        template <std::size_t>
        struct SizedUnsignedType;
#define SizedUnsignedTypeHelper( TYPE )        \
    template <>                                \
    struct SizedUnsignedType<sizeof( TYPE )> { \
        using type = TYPE;                     \
    }

        SizedUnsignedTypeHelper( std::uint8_t );
        SizedUnsignedTypeHelper( std::uint16_t );
        SizedUnsignedTypeHelper( std::uint32_t );
        SizedUnsignedTypeHelper( std::uint64_t );
#undef SizedUnsignedTypeHelper

        template <std::size_t sz>
        using SizedUnsignedType_t = typename SizedUnsignedType<sz>::type;

        template <typename T>
        using DoubleWidthUnsignedType_t = SizedUnsignedType_t<2 * sizeof( T )>;

        template <typename T>
        struct ExtendedMultResult {
            T upper;
            T lower;
            friend bool operator==( ExtendedMultResult const& lhs,
                                    ExtendedMultResult const& rhs ) {
                return lhs.upper == rhs.upper && lhs.lower == rhs.lower;
            }
        };

        // Returns 128 bit result of multiplying lhs and rhs
        constexpr ExtendedMultResult<std::uint64_t>
        extendedMult( std::uint64_t lhs, std::uint64_t rhs ) {
            // We use the simple long multiplication approach for
            // correctness, we can use platform specific builtins
            // for performance later.

            // Split the lhs and rhs into two 32bit "digits", so that we can
            // do 64 bit arithmetic to handle carry bits.
            //            32b    32b    32b    32b
            //     lhs                  L1     L2
            //   * rhs                  R1     R2
            //            ------------------------
            //                       |  R2 * L2  |
            //                 |  R2 * L1  |
            //                 |  R1 * L2  |
            //           |  R1 * L1  |
            //           -------------------------
            //           |  a  |  b  |  c  |  d  |

#define CarryBits( x ) ( x >> 32 )
#define Digits( x ) ( x & 0xFF'FF'FF'FF )

            auto r2l2 = Digits( rhs ) * Digits( lhs );
            auto r2l1 = Digits( rhs ) * CarryBits( lhs );
            auto r1l2 = CarryBits( rhs ) * Digits( lhs );
            auto r1l1 = CarryBits( rhs ) * CarryBits( lhs );

            // Sum to columns first
            auto d = Digits( r2l2 );
            auto c = CarryBits( r2l2 ) + Digits( r2l1 ) + Digits( r1l2 );
            auto b = CarryBits( r2l1 ) + CarryBits( r1l2 ) + Digits( r1l1 );
            auto a = CarryBits( r1l1 );

            // Propagate carries between columns
            c += CarryBits( d );
            b += CarryBits( c );
            a += CarryBits( b );

            // Remove the used carries
            c = Digits( c );
            b = Digits( b );
            a = Digits( a );

#undef CarryBits
#undef Digits

            return {
                a << 32 | b, // upper 64 bits
                c << 32 | d  // lower 64 bits
            };
        }

        template <typename UInt>
        constexpr ExtendedMultResult<UInt> extendedMult( UInt lhs, UInt rhs ) {
            static_assert( std::is_unsigned<UInt>::value,
                           "extendedMult can only handle unsigned integers" );
            static_assert( sizeof( UInt ) < sizeof( std::uint64_t ),
                           "Generic extendedMult can only handle types smaller "
                           "than uint64_t" );
            using WideType = DoubleWidthUnsignedType_t<UInt>;

            auto result = WideType( lhs ) * WideType( rhs );
            return {
                static_cast<UInt>( result >> ( CHAR_BIT * sizeof( UInt ) ) ),
                static_cast<UInt>( result & UInt( -1 ) ) };
        }


        template <typename TargetType,
                  typename Generator>
            std::enable_if_t<sizeof(typename Generator::result_type) >= sizeof(TargetType),
            TargetType> fillBitsFrom(Generator& gen) {
            using gresult_type = typename Generator::result_type;
            static_assert( std::is_unsigned<TargetType>::value, "Only unsigned integers are supported" );
            static_assert( Generator::min() == 0 &&
                           Generator::max() == static_cast<gresult_type>( -1 ),
                           "Generator must be able to output all numbers in its result type (effectively it must be a random bit generator)" );

            // We want to return the top bits from a generator, as they are
            // usually considered higher quality.
            constexpr auto generated_bits = sizeof( gresult_type ) * CHAR_BIT;
            constexpr auto return_bits = sizeof( TargetType ) * CHAR_BIT;

            return static_cast<TargetType>( gen() >>
                                            ( generated_bits - return_bits) );
        }

        template <typename TargetType,
                  typename Generator>
            std::enable_if_t<sizeof(typename Generator::result_type) < sizeof(TargetType),
            TargetType> fillBitsFrom(Generator& gen) {
            using gresult_type = typename Generator::result_type;
            static_assert( std::is_unsigned<TargetType>::value,
                           "Only unsigned integers are supported" );
            static_assert( Generator::min() == 0 &&
                           Generator::max() == static_cast<gresult_type>( -1 ),
                           "Generator must be able to output all numbers in its result type (effectively it must be a random bit generator)" );

            constexpr auto generated_bits = sizeof( gresult_type ) * CHAR_BIT;
            constexpr auto return_bits = sizeof( TargetType ) * CHAR_BIT;
            std::size_t filled_bits = 0;
            TargetType ret = 0;
            do {
                ret <<= generated_bits;
                ret |= gen();
                filled_bits += generated_bits;
            } while ( filled_bits < return_bits );

            return ret;
        }

        /*
         * Transposes numbers into unsigned type while keeping their ordering
         *
         * This means that signed types are changed so that the ordering is
         * [INT_MIN, ..., -1, 0, ..., INT_MAX], rather than order we would
         * get by simple casting ([0, ..., INT_MAX, INT_MIN, ..., -1])
         */
        template <typename OriginalType, typename UnsignedType>
        std::enable_if_t<std::is_signed<OriginalType>::value, UnsignedType>
        transposeToNaturalOrder( UnsignedType in ) {
            static_assert(
                sizeof( OriginalType ) == sizeof( UnsignedType ),
                "reordering requires the same sized types on both sides" );
            static_assert( std::is_unsigned<UnsignedType>::value,
                           "Input type must be unsigned" );
            // Assuming 2s complement (standardized in current C++), the
            // positive and negative numbers are already internally ordered,
            // and their difference is in the top bit. Swapping it orders
            // them the desired way.
            constexpr auto highest_bit =
                UnsignedType( 1 ) << ( sizeof( UnsignedType ) * CHAR_BIT - 1 );
            return static_cast<UnsignedType>( in ^ highest_bit );
        }



        template <typename OriginalType,
                  typename UnsignedType>
        std::enable_if_t<std::is_unsigned<OriginalType>::value, UnsignedType>
            transposeToNaturalOrder(UnsignedType in) {
            static_assert(
                sizeof( OriginalType ) == sizeof( UnsignedType ),
                "reordering requires the same sized types on both sides" );
            static_assert( std::is_unsigned<UnsignedType>::value, "Input type must be unsigned" );
            // No reordering is needed for unsigned -> unsigned
            return in;
        }
    } // namespace Detail
} // namespace Catch

#endif // CATCH_RANDOM_INTEGER_HELPERS_HPP_INCLUDED
