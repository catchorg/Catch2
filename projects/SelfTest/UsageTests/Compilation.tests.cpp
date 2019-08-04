/*
 *  Created by Martin on 17/02/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <type_traits>

// Setup for #1403 -- look for global overloads of operator << for classes
// in a different namespace.
#include <ostream>

namespace foo {
    struct helper_1403 {
        bool operator==(helper_1403) const { return true; }
    };
}

namespace bar {
    template <typename... Ts>
    struct TypeList {};
}

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#endif
std::ostream& operator<<(std::ostream& out, foo::helper_1403 const&) {
    return out << "[1403 helper]";
}
///////////////////////////////

#include "catch.hpp"

#include <cstring>

namespace { namespace CompilationTests {

#ifndef COMPILATION_TEST_HELPERS_INCLUDED // Don't compile this more than once per TU
#define COMPILATION_TEST_HELPERS_INCLUDED

    // Comparison operators can return non-booleans.
    // This is unusual, but should be supported.
    struct logic_t {
        logic_t operator< (logic_t) const { return {}; }
        logic_t operator<=(logic_t) const { return {}; }
        logic_t operator> (logic_t) const { return {}; }
        logic_t operator>=(logic_t) const { return {}; }
        logic_t operator==(logic_t) const { return {}; }
        logic_t operator!=(logic_t) const { return {}; }
        explicit operator bool() const { return true; }
    };


// This is a minimal example for an issue we have found in 1.7.0
    struct foo {
        int i;
    };

    template<typename T>
    bool operator==(const T &val, foo f) {
        return val == f.i;
    }

    struct Y {
        uint32_t v : 1;
    };

    void throws_int(bool b) {
        if (b) {
            throw 1;
        }
    }

    template<typename T>
    bool templated_tests(T t) {
        int a = 3;
        REQUIRE(a == t);
        CHECK(a == t);
        REQUIRE_THROWS(throws_int(true));
        CHECK_THROWS_AS(throws_int(true), int);
        REQUIRE_NOTHROW(throws_int(false));
#ifndef CATCH_CONFIG_DISABLE_MATCHERS
        REQUIRE_THAT("aaa", Catch::EndsWith("aaa"));
#endif
        return true;
    }

    struct A {
    };

    std::ostream &operator<<(std::ostream &o, const A &) { return o << 0; }

    struct B : private A {
        bool operator==(int) const { return true; }
    };

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif
#ifdef __GNUC__
// Note that because -~GCC~-, this warning cannot be silenced temporarily, by pushing diagnostic stack...
// Luckily it is firing in test files and thus can be silenced for the whole file, without losing much.
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

    B f();

    std::ostream g();

#ifdef __clang__
#pragma clang diagnostic pop
#endif

    template <typename, typename>
    struct Fixture_1245 {};

#endif

    TEST_CASE("#809") {
        foo f;
        f.i = 42;
        REQUIRE(42 == f);
    }


// ------------------------------------------------------------------
// Changes to REQUIRE_THROWS_AS made it stop working in a template in
// an unfixable way (as long as C++03 compatibility is being kept).
// To prevent these from happening in the future, this needs to compile

    TEST_CASE("#833") {
        REQUIRE(templated_tests<int>(3));
    }


// Test containing example where original stream insertable check breaks compilation


    TEST_CASE("#872") {
        A dummy;
        CAPTURE(dummy);
        B x;
        REQUIRE (x == 4);
    }


    TEST_CASE("#1027") {
        Y y{0};
        REQUIRE(y.v == 0);
        REQUIRE(0 == y.v);
    }

    // Comparison operators can return non-booleans.
    // This is unusual, but should be supported.
    TEST_CASE("#1147") {
        logic_t t1, t2;
        REQUIRE(t1 == t2);
        REQUIRE(t1 != t2);
        REQUIRE(t1 <  t2);
        REQUIRE(t1 >  t2);
        REQUIRE(t1 <= t2);
        REQUIRE(t1 >= t2);
    }

    // unsigned array
    TEST_CASE("#1238") {
        unsigned char uarr[] = "123";
        CAPTURE(uarr);
        signed char sarr[] = "456";
        CAPTURE(sarr);

        REQUIRE(std::memcmp(uarr, "123", sizeof(uarr)) == 0);
        REQUIRE(std::memcmp(sarr, "456", sizeof(sarr)) == 0);
    }

    TEST_CASE_METHOD((Fixture_1245<int, int>), "#1245", "[compilation]") {
        SUCCEED();
    }

    TEST_CASE("#1403", "[compilation]") {
        ::foo::helper_1403 h1, h2;
        REQUIRE(h1 == h2);
    }

    TEST_CASE("Optionally static assertions", "[compilation]") {
        STATIC_REQUIRE( std::is_void<void>::value );
        STATIC_REQUIRE_FALSE( std::is_void<int>::value );
    }

    TEST_CASE("#1548", "[compilation]") {
        using namespace bar;
        REQUIRE(std::is_same<TypeList<int>, TypeList<int>>::value);
    }

    // #925
    using signal_t = void (*) (void*);

    struct TestClass {
        signal_t testMethod_uponComplete_arg = nullptr;
    };

    namespace utility {
        inline static void synchronizing_callback( void * ) { }
    }

    TEST_CASE("#925: comparing function pointer to function address failed to compile", "[!nonportable]" ) {
        TestClass test;
        REQUIRE(utility::synchronizing_callback != test.testMethod_uponComplete_arg);
    }

    TEST_CASE( "#1027: Bitfields can be captured" ) {
        struct Y {
            uint32_t v : 1;
        };
        Y y{ 0 };
        REQUIRE( y.v == 0 );
        REQUIRE( 0 == y.v );
    }


}} // namespace CompilationTests

