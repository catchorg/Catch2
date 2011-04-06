/*
 *  TrickyTests.cpp
 *  Catch - Test
 *
 *  Created by Phil on 09/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "../catch.hpp"

namespace Catch
{
    template<>
    std::string toString<std::pair<int, int> >( const std::pair<int, int>& value )
    {
        std::ostringstream oss;
        oss << "std::pair( " << value.first << ", " << value.second << " )";
        return oss.str();
        
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "./succeeding/Tricky/std::pair", 
    "Parsing a std::pair"
)
{
    std::pair<int, int> aNicePair( 1, 2 );

    // !TBD: would be nice if this could compile without the extra parentheses
    REQUIRE( (std::pair<int, int>( 1, 2 )) == aNicePair );
    
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "./inprogress/failing/Tricky/trailing expression", 
    "Where the is more to the expression after the RHS"
)
{
    /*
    int a = 1;
    int b = 2;

    // This only captures part of the expression, but issues a warning about the rest
    REQUIRE( a == 2 || b == 2 );
     */
    WARN( "Uncomment the code in this test to check that it gives a sensible compiler error" );
}
///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "./inprogress/failing/Tricky/compound lhs", 
    "Where the LHS is not a simple value"
)
{
    /*
    int a = 1;
    int b = 2;
    
    // This only captures part of the expression, but issues a warning about the rest
    REQUIRE( a+1 == b-1 );
    */
    WARN( "Uncomment the code in this test to check that it gives a sensible compiler error" );
}

struct Opaque
{
    int val;
    bool operator ==( const Opaque& o ) const
    {
        return val == o.val;
    }
};

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "./failing/Tricky/non streamable type", 
    "A failing expression with a non streamable type is still captured" 
)
{
    
    Opaque o1, o2;
    o1.val = 7;
    o2.val = 8;
  
    CHECK( &o1 == &o2 );
    CHECK( o1 == o2 );
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(   
    "./failing/string literals", 
    "string literals of different sizes can be compared" 
)
{
    REQUIRE( std::string( "first" ) == "second" );
    
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(   
    "./succeeding/side-effects", 
    "An expression with side-effects should only be evaluated once" 
)
{
    int i = 7;
    
    REQUIRE( i++ == 7 );
    REQUIRE( i++ == 8 );
    
}

namespace A {
    struct X
    {
        X() : a(4), b(2), c(7) {}
        X(int v) : a(v), b(2), c(7) {}
        int a;
        int b;
        int c;
    };
}

namespace B {
    struct Y
    {
        Y() : a(4), b(2), c(7) {}
        Y(int v) : a(v), b(2), c(7) {}
        int a;
        int b;
        int c;
    };
}
bool operator==(const A::X& lhs, const B::Y& rhs)
{
    return (lhs.a == rhs.a);
}

bool operator==(const B::Y& lhs, const A::X& rhs)
{
    return (lhs.a == rhs.a);
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "./succeeding/koenig",
    "Operators at different namespace levels not hijacked by Koenig lookup"
)
{
    A::X x;
    B::Y y;
    REQUIRE( x == y );
}


namespace ObjectWithConversions
{    
    struct Object 
    {
        operator unsigned int() {return 0xc0000000;}
    };

    ///////////////////////////////////////////////////////////////////////////////
    TEST_CASE
    (
        "./succeeding/koenig",
        "Operators at different namespace levels not hijacked by Koenig lookup"
    )
    {
        
        Object o;
        
        bool ok = (0xc0000000 == o); // ok
        REQUIRE(ok);
        REQUIRE(0xc0000000 == o );	// doesn't compile (VC or GCC)
    }
}

namespace ObjectWithNonConstEqualityOperator 
{
    struct Test
    {
        Test( unsigned int v )
        : m_value(v) 
        {}
        
        bool operator==( const Test&rhs )
        { 
            return (m_value == rhs.m_value);
        }
        bool operator==( const Test&rhs ) const
        { 
            return (m_value != rhs.m_value);
        }
        unsigned int m_value;
    };
    
    TEST_CASE("./succeeding/non-const==", "Demonstrate that a non-const == is not used")
    {
        Test t( 1 );
        
        bool ok = (t == 1); // ok
        REQUIRE(ok);	
        REQUIRE( t == 1 );	// doesn't compile (VC or GCC)
    }
}

namespace EnumBitFieldTests
{
    enum Bits {bit0 = 0x0001, bit1 = 0x0002, bit2 = 0x0004, bit3 = 0x0008, bit1and2 = 0x0006,
        bit30 = 0x40000000, bit31 = 0x80000000,
        bit30and31 = 0xc0000000};

    TEST_CASE("./succeeding/enum/bits", "Test enum bit values")
    {
        REQUIRE( 0xc0000000 == bit30and31 );
    }
}
