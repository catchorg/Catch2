/*
 *  Created by Phil on 09/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifdef __clang__
#pragma clang diagnostic ignored "-Wpadded"
#endif

#include "catch.hpp"

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#endif

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
    "Parsing a std::pair",
    "[Tricky][std::pair]"
)
{
    std::pair<int, int> aNicePair( 1, 2 );

    REQUIRE( (std::pair<int, int>( 1, 2 )) == aNicePair );    
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "Where there is more to the expression after the RHS",
    "[Tricky][failing][.]"
)
{
//    int a = 1, b = 2;
//    REQUIRE( a == 2 || b == 2 );
    WARN( "Uncomment the code in this test to check that it gives a sensible compiler error" );
}
///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(
    "Where the LHS is not a simple value",
    "[Tricky][failing][.]"
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
    "A failing expression with a non streamable type is still captured",
    "[Tricky][failing][.]"
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
    "string literals of different sizes can be compared",
    "[Tricky][failing][.]"
)
{
    REQUIRE( std::string( "first" ) == "second" );
    
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE
(   
    "An expression with side-effects should only be evaluated once",
    "[Tricky]"
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

inline bool operator==(const A::X& lhs, const B::Y& rhs)
{
    return (lhs.a == rhs.a);
}

inline bool operator==(const B::Y& lhs, const A::X& rhs)
{
    return (lhs.a == rhs.a);
}


///////////////////////////////////////////////////////////////////////////////
/* This, currently, does not compile with LLVM
TEST_CASE
(
    "Operators at different namespace levels not hijacked by Koenig lookup"
    "[Tricky]"
)
{
    A::X x;
    B::Y y;
    REQUIRE( x == y );
}
*/

namespace ObjectWithConversions
{    
    struct Object 
    {
        operator unsigned int() {return 0xc0000000;}
    };

    ///////////////////////////////////////////////////////////////////////////////
    TEST_CASE
    (
        "Operators at different namespace levels not hijacked by Koenig lookup",
        "[Tricky]"
    )
    {        
        Object o;
        REQUIRE(0xc0000000 == o );
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
    
    TEST_CASE("Demonstrate that a non-const == is not used", "[Tricky]" )
    {
        Test t( 1 );
        REQUIRE( t == 1u );
    }
}

namespace EnumBitFieldTests
{
    enum Bits {bit0 = 0x0001, bit1 = 0x0002, bit2 = 0x0004, bit3 = 0x0008, bit1and2 = 0x0006,
        bit30 = 0x40000000, bit31 = 0x80000000,
        bit30and31 = 0xc0000000};

    TEST_CASE( "Test enum bit values", "[Tricky]" )
    {
        REQUIRE( 0xc0000000 == bit30and31 );
    }
}

struct Obj
{
    Obj():prop(&p){}
    
    int p;
    int* prop;
};

TEST_CASE("boolean member", "[Tricky]")
{
    Obj obj;
    REQUIRE( obj.prop != NULL );
}

// Tests for a problem submitted by Ralph McArdell
//
// The static bool value should not need to be defined outside the
// struct it is declared in - but when evaluating it in a deduced
// context it appears to require the extra definition.
// The issue was fixed by adding bool overloads to bypass the
// templates that were there to deduce it.
template <bool B>
struct is_true
{
    static const bool value = B;
};

TEST_CASE( "(unimplemented) static bools can be evaluated", "[Tricky]" )
{
    SECTION("compare to true","")
    {
        REQUIRE( is_true<true>::value == true );
        REQUIRE( true == is_true<true>::value );
    }
    SECTION("compare to false","")
    {
        REQUIRE( is_true<false>::value == false );
        REQUIRE( false == is_true<false>::value );
    }

    SECTION("negation", "")
    {
        REQUIRE( !is_true<false>::value );
    }

    SECTION("double negation","")
    {
        REQUIRE( !!is_true<true>::value );
    }

    SECTION("direct","")
    {
        REQUIRE( is_true<true>::value );
        REQUIRE_FALSE( is_true<false>::value );
    }
}

// Uncomment these tests to produce an error at test registration time
/*
TEST_CASE( "Tests with the same name are not allowed", "[Tricky]" )
{
    
}
TEST_CASE( "Tests with the same name are not allowed", "[Tricky]" )
{
    
}
*/

struct Boolable
{
    explicit Boolable( bool value ) : m_value( value ) {}

    operator Catch::SafeBool::type() const {
        return Catch::SafeBool::makeSafe( m_value );
    }

    bool m_value;
};

TEST_CASE( "Objects that evaluated in boolean contexts can be checked", "[Tricky][SafeBool]" )
{
    Boolable True( true );
    Boolable False( false );

    CHECK( True );
    CHECK( !False );
    CHECK_FALSE( False );
}

TEST_CASE( "Assertions then sections", "[Tricky]" )
{
    // This was causing a failure due to the way the console reporter was handling
    // the current section
    
    REQUIRE( Catch::alwaysTrue() );
    
    SECTION( "A section", "" )
    {
        REQUIRE( Catch::alwaysTrue() );
        
        SECTION( "Another section", "" )
        {
            REQUIRE( Catch::alwaysTrue() );
        }
        SECTION( "Another other section", "" )
        {
            REQUIRE( Catch::alwaysTrue() );
        }
    }
}

struct Awkward
{
    operator int() const { return 7; }
};

TEST_CASE( "non streamable - with conv. op", "[Tricky]" )
{
    Awkward awkward;
    std::string s = Catch::toString( awkward );
    REQUIRE( s == "7" );
}

inline void foo() {}

typedef void (*fooptr_t)();

TEST_CASE( "Comparing function pointers", "[Tricky][function pointer]" )
{
    // This was giving a warning in VS2010
    // #179
    fooptr_t a = foo;

    REQUIRE( a );
    REQUIRE( a == &foo );
}

struct S
{
    void f() {}
};


TEST_CASE( "Comparing member function pointers", "[Tricky][member function pointer]" )
{
    typedef void (S::*MF)();
    MF m = &S::f;

    CHECK( m == &S::f );
}

class ClassName {};

TEST_CASE( "pointer to class", "[Tricky]" )
{
   ClassName *p = 0;
   REQUIRE( p == 0 );
}

#ifdef CATCH_CONFIG_CPP11_NULLPTR

#include <memory>

TEST_CASE( "null_ptr", "[Tricky][c++11]" )
{
    std::unique_ptr<int> ptr;
    REQUIRE(ptr.get() == nullptr);
}

#endif

TEST_CASE( "X/level/0/a", "[Tricky]" )      { SUCCEED(""); }
TEST_CASE( "X/level/0/b", "[Tricky][fizz]" ){ SUCCEED(""); }
TEST_CASE( "X/level/1/a", "[Tricky]" )      { SUCCEED(""); }
TEST_CASE( "X/level/1/b", "[Tricky]" )      { SUCCEED(""); }
