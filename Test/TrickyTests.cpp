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
