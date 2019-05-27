/*
 *  Created by Phil on 09/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"
#include <array>

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

template< typename T, size_t V>
struct Template_Foo_2 {
    size_t size() { return V; }
};

template <int V>
struct Nttp_Fixture{
    int value = V;
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

TEMPLATE_TEST_CASE_METHOD_SIG(Nttp_Fixture, "A TEMPLATE_TEST_CASE_METHOD_SIG based test run that succeeds", "[class][template][nttp]",((int V), V), 1, 3, 6) {
    REQUIRE(Nttp_Fixture<V>::value > 0);
}

TEMPLATE_PRODUCT_TEST_CASE_METHOD(Template_Fixture_2, "A TEMPLATE_PRODUCT_TEST_CASE_METHOD based test run that succeeds","[class][template][product]",(std::vector,Template_Foo),(int,float))
{
    REQUIRE( Template_Fixture_2<TestType>::m_a.size() == 0 );
}

TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG(Template_Fixture_2, "A TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG based test run that succeeds", "[class][template][product][nttp]", ((typename T, size_t S), T, S),(std::array, Template_Foo_2), ((int,2), (float,6)))
{
    REQUIRE(Template_Fixture_2<TestType>{}.m_a.size() >= 2);
}

using MyTypes = std::tuple<int, char, double>;
TEMPLATE_LIST_TEST_CASE_METHOD(Template_Fixture, "Template test case method with test types specified inside std::tuple", "[class][template][list]", MyTypes)
{
    REQUIRE( Template_Fixture<TestType>::m_a == 1 );
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

    TEMPLATE_TEST_CASE_METHOD_SIG(Nttp_Fixture, "A TEMPLATE_TEST_CASE_METHOD_SIG based test run that fails", "[.][class][template][nttp][failing]", ((int V), V), 1, 3, 6) {
        REQUIRE(Nttp_Fixture<V>::value == 0);
    }

    TEMPLATE_PRODUCT_TEST_CASE_METHOD(Template_Fixture_2, "A TEMPLATE_PRODUCT_TEST_CASE_METHOD based test run that fails","[.][class][template][product][failing]",(std::vector,Template_Foo),(int,float))
    {
        REQUIRE( Template_Fixture_2<TestType>::m_a.size() == 1 );
    }

    TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG(Template_Fixture_2, "A TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG based test run that fails", "[.][class][template][product][nttp][failing]", ((typename T, size_t S), T, S), (std::array, Template_Foo_2), ((int, 2), (float, 6)))
    {
        REQUIRE(Template_Fixture_2<TestType>{}.m_a.size() < 2);
    }
}



}} // namespace ClassTests
