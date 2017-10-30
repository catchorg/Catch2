/*
 *  Created by Phil on 28/04/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "Some simple comparisons between doubles",
    "[Approx]"
)
{
    double d = 1.23;

    REQUIRE( d == Approx( 1.23 ) );
    REQUIRE( d != Approx( 1.22 ) );
    REQUIRE( d != Approx( 1.24 ) );

    REQUIRE( Approx( d ) == 1.23 );
    REQUIRE( Approx( d ) != 1.22 );
    REQUIRE( Approx( d ) != 1.24 );

    REQUIRE( 0 == Approx(0) );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "Approximate comparisons with different epsilons",
    "[Approx]"
 )
{
    double d = 1.23;

    REQUIRE( d != Approx( 1.231 ) );
    REQUIRE( d == Approx( 1.231 ).epsilon( 0.1 ) );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
 "Less-than inequalities with different epsilons",
 "[Approx]"
)
{
  double d = 1.23;

  REQUIRE( d <= Approx( 1.24 ) );
  REQUIRE( d <= Approx( 1.23 ) );
  REQUIRE_FALSE( d <= Approx( 1.22 ) );
  REQUIRE( d <= Approx( 1.22 ).epsilon(0.1) );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
 "Greater-than inequalities with different epsilons",
 "[Approx]"
)
{
  double d = 1.23;

  REQUIRE( d >= Approx( 1.22 ) );
  REQUIRE( d >= Approx( 1.23 ) );
  REQUIRE_FALSE( d >= Approx( 1.24 ) );
  REQUIRE( d >= Approx( 1.24 ).epsilon(0.1) );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "Approximate comparisons with floats",
    "[Approx]"
)
{
    REQUIRE( 1.23f == Approx( 1.23f ) );
    REQUIRE( 0.0f == Approx( 0.0f ) );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "Approximate comparisons with ints",
    "[Approx]"
)
{
    REQUIRE( 1 == Approx( 1 ) );
    REQUIRE( 0 == Approx( 0 ) );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "Approximate comparisons with mixed numeric types",
    "[Approx]"
)
{
    const double dZero = 0;
    const double dSmall = 0.00001;
    const double dMedium = 1.234;

    REQUIRE( 1.0f == Approx( 1 ) );
    REQUIRE( 0 == Approx( dZero) );
    REQUIRE( 0 == Approx( dSmall ).epsilon( 0.001 ) );
    REQUIRE( 1.234f == Approx( dMedium ) );
    REQUIRE( dMedium == Approx( 1.234f ) );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "Use a custom approx",
    "[Approx][custom]"
)
{
    double d = 1.23;

    Approx approx = Approx::custom().epsilon( 0.005 );

    REQUIRE( d == approx( 1.23 ) );
    REQUIRE( d == approx( 1.22 ) );
    REQUIRE( d == approx( 1.24 ) );
    REQUIRE( d != approx( 1.25 ) );

    REQUIRE( approx( d ) == 1.23 );
    REQUIRE( approx( d ) == 1.22 );
    REQUIRE( approx( d ) == 1.24 );
    REQUIRE( approx( d ) != 1.25 );
}

inline double divide( double a, double b ) {
    return a/b;
}

TEST_CASE( "Approximate PI", "[Approx][PI]" )
{
    REQUIRE( divide( 22, 7 ) == Approx( 3.141 ).epsilon( 0.001 ) );
    REQUIRE( divide( 22, 7 ) != Approx( 3.141 ).epsilon( 0.0001 ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST_CASE( "Absolute margin", "[Approx]" ) {
    REQUIRE( 104.0 != Approx(100.0) );
    REQUIRE( 104.0 == Approx(100.0).margin(5) );
    REQUIRE( 104.0 == Approx(100.0).margin(4) );
    REQUIRE( 104.0 != Approx(100.0).margin(3) );
    REQUIRE( 100.3 != Approx(100.0) );
    REQUIRE( 100.3 == Approx(100.0).margin(0.5) );
}

TEST_CASE("Approx with exactly-representable margin", "[Approx]") {
    CHECK( 0.25f == Approx(0.0f).margin(0.25f) );

    CHECK( 0.0f == Approx(0.25f).margin(0.25f) );
    CHECK( 0.5f == Approx(0.25f).margin(0.25f) );

    CHECK( 245.0f == Approx(245.25f).margin(0.25f) );
    CHECK( 245.5f == Approx(245.25f).margin(0.25f) );
}

////////////////////////////////////////////////////////////////////////////////

#if defined(CATCH_CONFIG_CPP11_TYPE_TRAITS)
class StrongDoubleTypedef
{
  double d_ = 0.0;

  public:
    explicit StrongDoubleTypedef(double d) : d_(d) {}
    explicit operator double() const { return d_; }
};

inline std::ostream& operator<<( std::ostream& os, StrongDoubleTypedef td ) {
    return os << "StrongDoubleTypedef(" << static_cast<double>(td) << ")";
}

TEST_CASE
(
 "Comparison with explicitly convertible types",
 "[Approx][c++11]"
)
{
  StrongDoubleTypedef td(10.0);

  REQUIRE(td == Approx(10.0));
  REQUIRE(Approx(10.0) == td);

  REQUIRE(td != Approx(11.0));
  REQUIRE(Approx(11.0) != td);

  REQUIRE(td <= Approx(10.0));
  REQUIRE(td <= Approx(11.0));
  REQUIRE(Approx(10.0) <= td);
  REQUIRE(Approx(9.0) <= td);

  REQUIRE(td >= Approx(9.0));
  REQUIRE(td >= Approx(10.0));
  REQUIRE(Approx(10.0) >= td);
  REQUIRE(Approx(11.0) >= td);

}
#endif

////////////////////////////////////////////////////////////////////////////////
