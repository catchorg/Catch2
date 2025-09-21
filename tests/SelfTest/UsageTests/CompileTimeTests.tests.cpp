
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_constexpr_section.hpp>

// In C++14, CONSTEXPR_SECTIONS are not supported.
// For consistency between test results across C++ standard versions, we
// redefine here CONSTEXPR_SECTION as a SECTION with a mock assertion.
#ifndef CATCH_CONFIG_CONSTEXPR_SECTIONS

#    if defined( __GNUC__ ) && !defined( __clang__ ) && !defined( __ICC ) && \
        !defined( __CUDACC__ ) && !defined( __LCC__ ) &&                     \
        !defined( __NVCOMPILER ) && __GNUC__ <= 9
#        define SUPPRESS_MISLEADING_INDENTATION_WARNINGS \
            _Pragma( "GCC diagnostic ignored \"-Wmisleading-indentation\"" )
#    else
#        define SUPPRESS_MISLEADING_INDENTATION_WARNINGS
#    endif

#    undef CONSTEXPR_SECTION
#    define CONSTEXPR_SECTION( ... )                            \
        CATCH_INTERNAL_START_WARNINGS_SUPPRESSION               \
        SUPPRESS_MISLEADING_INDENTATION_WARNINGS                \
        SECTION( "[Passed during compilation] " __VA_ARGS__ ) { \
            Catch::ConstexprSection::addFakeAssertion();        \
        }                                                       \
        CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION
#endif

TEST_CASE( "Simple compile time section" ) {
    CONSTEXPR_SECTION( "Simple compile time section" ) {
        int x = 5;
        x *= 10;
        CONSTEXPR_REQUIRE( x == 50 );
        CONSTEXPR_REQUIRE_FALSE( x != 50 );
    };
}

TEST_CASE( "CONSTEXPR_SECTION, CONSTEXPR_REQUIRE, "
           "CONSTEXPR_REQUIRE_FALSE behavior during compilation" ) {
    CONSTEXPR_SECTION( "First" ) {
        int x = 19;
        CONSTEXPR_REQUIRE( x == 19 );
        x *= 10;
        CONSTEXPR_REQUIRE( x == 190 );

        bool b = false;
        CONSTEXPR_REQUIRE_FALSE( b );
        b = true;
        CONSTEXPR_REQUIRE( b );
    };

    CONSTEXPR_SECTION( "Second", "Having a description should be ok" ) {
        double x = 1.1;
        x *= 10;
        CONSTEXPR_REQUIRE( x > 10.0 );
        CONSTEXPR_REQUIRE_FALSE( x < -9.0 );
    };

    CONSTEXPR_SECTION( "Third" ){
        // Keeping the implementation should be perfectly fine
    };
}

TEST_CASE( "CONSTEXPR_REQUIRE, CONSTEXPR_REQUIRE_FALSE should evaluate "
           "the expression only once at runtime in case of success" ) {
    SECTION( "Unary -> Expression should be evaluated only once" ) {
        int r = 5;
        auto lambda = [&] {
            r *= 10;
            return r;
        };
        CONSTEXPR_REQUIRE( ( lambda() == 50 ) );
        CONSTEXPR_REQUIRE( ( lambda() == 500 ) );
        const bool result = lambda() == 5000;
        CONSTEXPR_REQUIRE( result );
        CONSTEXPR_REQUIRE_FALSE( !result );

        CONSTEXPR_REQUIRE( true );
        CONSTEXPR_REQUIRE_FALSE( false );
        SUCCEED( "Silent empty section warning (at runtime, "
                 "CONSTEXPR_REQUIRE does nothing if passed)" );
    }
    SECTION( "Binary -> Expression should be evaluated only once" ) {
        int r = 5;
        auto lambda = [&] {
            r *= 10;
            return r;
        };
        CONSTEXPR_REQUIRE( lambda() == 50 );
        CONSTEXPR_REQUIRE( lambda() == 500 );
        SUCCEED( "Silent empty section warning (at runtime, "
                 "CONSTEXPR_REQUIRE does nothing if passed)" );
    }
    SUCCEED( "Silent empty section warning" );
}

TEST_CASE( "CONSTEXPR_REQUIRE, CONSTEXPR_REQUIRE_FALSE should evaluate "
           "the expression only once at runtime in case of failure",
           "[!shouldfail]" ) {
    SECTION( "Unary -> Expression should be evaluated only once" ) {
        int numberOfCalls = 0;
        auto lambda = [&] {
            ++numberOfCalls;
            REQUIRE( numberOfCalls < 2 ); // lambda() was evaluated
            return 50;
        };
        CONSTEXPR_REQUIRE( ( lambda() != 50 ) );
    }
    SECTION( "Binary -> Expression should be evaluated only once" ) {
        int numberOfCalls = 0;
        auto lambda = [&] {
            ++numberOfCalls;
            REQUIRE( numberOfCalls < 2 ); // lambda() was evaluated twice
            return 50;
        };
        CONSTEXPR_REQUIRE( lambda() != 50 );
    }
    SUCCEED( "Silent empty section warning" );
}
