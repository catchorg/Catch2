/*
 *  Created by Phil on 09/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

namespace{ namespace ClassTests {

#ifndef CLASS_TEST_HELPERS_INCLUDED // Don't compile this more than once per TU
#define CLASS_TEST_HELPERS_INCLUDED

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

struct Fixture
{
    Fixture() : m_a( 1 ) {}

    int m_a;
};

template< typename T >
struct Template_Fixture {
    Template_Fixture(): m_a(1) {}

    T m_a;
};

template<typename T>
struct Template_Fixture_2 {
    Template_Fixture_2() {}

    T m_a;
};

template< typename T>
struct Template_Foo {
    size_t size() { return 0; }
};

#endif



METHOD_AS_TEST_CASE( TestClass::succeedingCase, "A METHOD_AS_TEST_CASE based test run that succeeds", "[class]" )
METHOD_AS_TEST_CASE( TestClass::failingCase, "A METHOD_AS_TEST_CASE based test run that fails", "[.][class][failing]" )

TEST_CASE_METHOD( Fixture, "A TEST_CASE_METHOD based test run that succeeds", "[class]" )
{
    REQUIRE( m_a == 1 );
}

TEMPLATE_TEST_CASE_METHOD(Template_Fixture, "A TEMPLATE_TEST_CASE_METHOD based test run that succeeds", "[class][template]", int, float, double) {
    REQUIRE( Template_Fixture<TestType>::m_a == 1 );
}

TEMPLATE_PRODUCT_TEST_CASE_METHOD(Template_Fixture_2, "A TEMPLATE_PRODUCT_TEST_CASE_METHOD based test run that succeeds","[class][template][product]",(std::vector,Template_Foo),(int,float))
{
    REQUIRE( Template_Fixture_2<TestType>::m_a.size() == 0 );
}

// We should be able to write our tests within a different namespace
namespace Inner
{
    TEST_CASE_METHOD( Fixture, "A TEST_CASE_METHOD based test run that fails", "[.][class][failing]" )
    {
        REQUIRE( m_a == 2 );
    }

    TEMPLATE_TEST_CASE_METHOD(Template_Fixture,"A TEMPLATE_TEST_CASE_METHOD based test run that fails", "[.][class][template][failing]", int, float, double)
    {
        REQUIRE( Template_Fixture<TestType>::m_a == 2 );
    }

    TEMPLATE_PRODUCT_TEST_CASE_METHOD(Template_Fixture_2, "A TEMPLATE_PRODUCT_TEST_CASE_METHOD based test run that fails","[.][class][template][product][failing]",(std::vector,Template_Foo),(int,float))
    {
        REQUIRE( Template_Fixture_2<TestType>::m_a.size() == 1 );
    }
}



}} // namespace ClassTests
