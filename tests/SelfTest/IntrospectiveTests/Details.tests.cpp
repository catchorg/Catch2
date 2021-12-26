/*
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_case_insensitive_comparisons.hpp>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4702) // unreachable code in the macro expansions
#endif

TEST_CASE("Check that our error handling macros throw the right exceptions", "[!throws][internals][approvals]") {
    REQUIRE_THROWS_AS(CATCH_INTERNAL_ERROR(""), std::logic_error);
    REQUIRE_THROWS_AS(CATCH_ERROR(""), std::domain_error);
    REQUIRE_THROWS_AS(CATCH_RUNTIME_ERROR(""), std::runtime_error);
    REQUIRE_THROWS_AS([](){CATCH_ENFORCE(false, "");}(), std::domain_error);
    REQUIRE_NOTHROW([](){CATCH_ENFORCE(true, "");}());
}

#if defined(_MSC_VER)
#pragma warning(pop) // unreachable code in the macro expansions
#endif

TEST_CASE("CaseInsensitiveLess is case insensitive", "[comparisons][string-case]") {
    Catch::Detail::CaseInsensitiveLess lt;
    SECTION( "Degenerate cases" ) {
        REQUIRE( lt( "", "a" ) );
        REQUIRE_FALSE( lt( "a", "a" ) );
        REQUIRE_FALSE( lt( "", "" ) );
    }
    SECTION("Plain comparisons") {
        REQUIRE( lt( "a", "b" ) );
        REQUIRE( lt( "a", "B" ) );
        REQUIRE( lt( "A", "b" ) );
        REQUIRE( lt( "A", "B" ) );
    }
}

TEST_CASE( "CaseInsensitiveEqualsTo is case insensitive",
           "[comparisons][string-case]" ) {
    Catch::Detail::CaseInsensitiveEqualTo eq;
    SECTION( "Degenerate cases" ) {
        REQUIRE( eq( "", "" ) );
        REQUIRE_FALSE( eq( "", "a" ) );
    }
    SECTION( "Plain comparisons" ) {
        REQUIRE( eq( "a", "a" ) );
        REQUIRE( eq( "a", "A" ) );
        REQUIRE( eq( "A", "a" ) );
        REQUIRE( eq( "A", "A" ) );
        REQUIRE_FALSE( eq( "a", "b" ) );
        REQUIRE_FALSE( eq( "a", "B" ) );
    }
}
