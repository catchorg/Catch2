/*
 *  Created by Phil on 09/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

namespace
{
    class TestClass
    {
        std::string s;
        
    public:
        TestClass()
        : s( "hello" )
        {}
        
        void succeedingCase()
        {            
            REQUIRE( s == "hello" );
        }
        void failingCase()
        {            
            REQUIRE( s == "world" );
        }
    };
}


METHOD_AS_TEST_CASE( TestClass::succeedingCase, "./succeeding/TestClass/succeedingCase", "A method based test run that succeeds [class]" )
METHOD_AS_TEST_CASE( TestClass::failingCase, "./failing/TestClass/failingCase", "A method based test run that fails [class]" )


struct Fixture
{
    Fixture() : m_a( 1 ) {}
    
    int m_a;
};

TEST_CASE_METHOD( Fixture, "./succeeding/Fixture/succeedingCase", "A method based test run that succeeds [class]" )
{   
    REQUIRE( m_a == 1 );        
}

// We should be able to write our tests within a different namespace
namespace Inner
{
    TEST_CASE_METHOD( Fixture, "./failing/Fixture/failingCase", "A method based test run that fails [class]" )
    {   
        REQUIRE( m_a == 2 );        
    }
}
