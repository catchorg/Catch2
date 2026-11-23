//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#define CATCH_CONFIG_TEMPLATE_LIST_TEST_CASE_SUFFIX(Type, ListName, Index) \
    ++suffixCalls, typeSuffix<Type>(ListName, Index)
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_case_info.hpp>
#include <catch2/interfaces/catch_interfaces_capture.hpp>
#include <catch2/interfaces/catch_interfaces_registry_hub.hpp>
#include <catch2/interfaces/catch_interfaces_testcase.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace {
    int suffixCalls = 0;

    template <typename...> struct TypeList;
    using Types = TypeList<int&, double, int&>;
    using FixtureTypes = TypeList<int&, double>;

    template <typename T> char const* typeName();
    template <> char const* typeName<int&>() { return "int&"; }
    template <> char const* typeName<double>() { return "double"; }

    template <typename T>
    std::string typeSuffix( Catch::StringRef listName, std::size_t index ) {
        if ( listName == "Types" && index == 2 ) {
            return std::string( typeName<T>() ) + " again";
        }
        return typeName<T>();
    }

    template <typename T> struct Fixture {
        Catch::StringRef expectedSuffix = typeName<T>();
    };
} // namespace

TEMPLATE_LIST_TEST_CASE( "List test", "[list]", Types ) {
    auto const name = Catch::getResultCapture().getCurrentTestName();
    CHECK( name.find( typeName<TestType>() ) != std::string::npos );
}

TEMPLATE_LIST_TEST_CASE_METHOD( Fixture, "List fixture test", "[list]", FixtureTypes ) {
    auto const name = Catch::getResultCapture().getCurrentTestName();
    CHECK( name == "List fixture test - " + this->expectedSuffix );
}

TEST_CASE( "Type list suffixes form the registered names" ) {
    std::vector<std::string> names;
    for ( auto const& test : Catch::getRegistryHub().getTestCaseRegistry().getAllTests() ) {
        names.push_back( test.getTestCaseInfo().name );
    }

    std::vector<std::string> const expected{
        "List test - int&",
        "List test - double",
        "List test - int& again",
        "List fixture test - int&",
        "List fixture test - double",
        "Type list suffixes form the registered names",
    };
    CHECK( names == expected );
    CHECK( suffixCalls == 5 );
}
