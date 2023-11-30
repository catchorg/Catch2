
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#if defined( __GNUC__ ) || defined( __clang__ )
#    pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

// `CATCH_CONFIG_NAME_SEPARATOR` should control how parameterized tests are named.
// It must be defined before the macro header is included.
#define CATCH_CONFIG_NAME_SEPARATOR "--"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <array>

namespace {

    template <typename T> struct Template_Fixture {
        Template_Fixture(): m_a( 1 ) {}

        T m_a;
    };

    template <typename T> struct Template_Fixture_2 {
        Template_Fixture_2() {}

        T m_a;
    };

    template <typename T> struct Template_Foo {
        size_t size() { return 0; }
    };

    template <typename T, size_t V> struct Template_Foo_2 {
        size_t size() { return V; }
    };

    template <int V> struct Nttp_Fixture { int value = V; };

} // end unnamed namespace

TEMPLATE_TEST_CASE_METHOD(Template_Fixture, "A TEMPLATE_TEST_CASE_METHOD based test with a custom separator", "[class][template][custom_separator]", int, float, double) {
    REQUIRE( Template_Fixture<TestType>::m_a == 1 );
}

TEMPLATE_TEST_CASE_METHOD_SIG(Nttp_Fixture, "A TEMPLATE_TEST_CASE_METHOD_SIG based test with a custom separator", "[class][template][nttp][custom_separator]",((int V), V), 1, 3, 6) {
    REQUIRE(Nttp_Fixture<V>::value > 0);
}

TEMPLATE_PRODUCT_TEST_CASE_METHOD(Template_Fixture_2, "A TEMPLATE_PRODUCT_TEST_CASE_METHOD based test with a custom separator","[class][template][product][custom_separator]",(std::vector,Template_Foo),(int,float))
{
    REQUIRE( Template_Fixture_2<TestType>::m_a.size() == 0 );
}

TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG(Template_Fixture_2, "A TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG based test with a custom separator", "[class][template][product][nttp][custom_separator]", ((typename T, size_t S), T, S),(std::array, Template_Foo_2), ((int,2), (float,6)))
{
    REQUIRE(Template_Fixture_2<TestType>{}.m_a.size() >= 2);
}

using MyTypes = std::tuple<int, char, double>;
TEMPLATE_LIST_TEST_CASE_METHOD(Template_Fixture, "Template test case method with test types specified inside std::tuple with a custom separator", "[class][template][list][custom_separator]", MyTypes)
{
    REQUIRE( Template_Fixture<TestType>::m_a == 1 );
}
