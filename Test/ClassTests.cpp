/*
 *  ClassTests.cpp
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
            EXPECT( s == "hello" );
        }
        void failingCase()
        {            
            EXPECT( s == "world" );
        }
    };
}
METHOD_AS_TEST_CASE( TestClass::succeedingCase, "succeeding/TestClass/succeedingCase", "A method based test run that succeeds" );
METHOD_AS_TEST_CASE( TestClass::failingCase, "failing/TestClass/failingCase", "A method based test run that fails" );
