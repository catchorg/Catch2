
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <helpers/type_with_lit_0_comparisons.hpp>

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
static std::ostream& operator<<(std::ostream& out, foo::helper_1403 const&) {
    return out << "[1403 helper]";
}
///////////////////////////////

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <cstring>

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


static void throws_int(bool b) {
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
    REQUIRE_THAT("aaa", Catch::Matchers::EndsWith("aaa"));
    return true;
}

struct A {};

static std::ostream &operator<<(std::ostream &o, const A &) { return o << 0; }

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

// This is a minimal example for an issue we have found in 1.7.0
struct dummy_809 {
    int i;
};

template<typename T>
bool operator==(const T& val, dummy_809 f) {
    return val == f.i;
}

TEST_CASE("#809") {
    dummy_809 f;
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

TEST_CASE("#1027: Bitfields can be captured") {
    struct Y {
        uint32_t v : 1;
    };
    Y y{ 0 };
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
    STATIC_CHECK( std::is_void<void>::value );
    STATIC_CHECK_FALSE( std::is_void<int>::value );
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

#if defined (_MSC_VER)
#pragma warning(push)
// The function pointer comparison below triggers warning because of
// calling conventions
#pragma warning(disable:4244)
#endif
    TEST_CASE("#925: comparing function pointer to function address failed to compile", "[!nonportable]" ) {
        TestClass test;
        REQUIRE(utility::synchronizing_callback != test.testMethod_uponComplete_arg);
    }
#if defined (_MSC_VER)
#pragma warning(pop)
#endif

TEST_CASE( "#1319: Sections can have description (even if it is not saved",
               "[compilation]" ) {
    SECTION( "SectionName", "This is a long form section description" ) {
        SUCCEED();
    }
}

TEST_CASE("Lambdas in assertions") {
    REQUIRE([]() { return true; }());
}

namespace {
    struct HasBitOperators {
        int value;

        friend HasBitOperators operator| (HasBitOperators lhs, HasBitOperators rhs) {
            return { lhs.value | rhs.value };
        }
        friend HasBitOperators operator& (HasBitOperators lhs, HasBitOperators rhs) {
            return { lhs.value & rhs.value };
        }
        friend HasBitOperators operator^ (HasBitOperators lhs, HasBitOperators rhs) {
            return { lhs.value ^ rhs.value };
        }
        explicit operator bool() const {
            return !!value;
        }

        friend std::ostream& operator<<(std::ostream& out, HasBitOperators val) {
            out << "Val: " << val.value;
            return out;
        }
    };
}

TEST_CASE("Assertion macros support bit operators and bool conversions", "[compilation][bitops]") {
    HasBitOperators lhs{ 1 }, rhs{ 2 };
    REQUIRE(lhs | rhs);
    REQUIRE_FALSE(lhs & rhs);
    REQUIRE(HasBitOperators{ 1 } & HasBitOperators{ 1 });
    REQUIRE(lhs ^ rhs);
    REQUIRE_FALSE(lhs ^ lhs);
}

namespace {
    struct ImmovableType {
        ImmovableType() = default;

        ImmovableType(ImmovableType const&) = delete;
        ImmovableType& operator=(ImmovableType const&) = delete;
        ImmovableType(ImmovableType&&) = delete;
        ImmovableType& operator=(ImmovableType&&) = delete;

        friend bool operator==(ImmovableType const&, ImmovableType const&) {
            return true;
        }
    };
}

TEST_CASE("Immovable types are supported in basic assertions", "[compilation][.approvals]") {
    REQUIRE(ImmovableType{} == ImmovableType{});
}

namespace adl {

struct always_true {
    explicit operator bool() const { return true; }
};

#define COMPILATION_TEST_DEFINE_UNIVERSAL_OPERATOR(op) \
template <class T, class U> \
auto operator op (T&&, U&&) { \
    return always_true{}; \
}

COMPILATION_TEST_DEFINE_UNIVERSAL_OPERATOR(==)
COMPILATION_TEST_DEFINE_UNIVERSAL_OPERATOR(!=)
COMPILATION_TEST_DEFINE_UNIVERSAL_OPERATOR(<)
COMPILATION_TEST_DEFINE_UNIVERSAL_OPERATOR(>)
COMPILATION_TEST_DEFINE_UNIVERSAL_OPERATOR(<=)
COMPILATION_TEST_DEFINE_UNIVERSAL_OPERATOR(>=)
COMPILATION_TEST_DEFINE_UNIVERSAL_OPERATOR(|)
COMPILATION_TEST_DEFINE_UNIVERSAL_OPERATOR(&)
COMPILATION_TEST_DEFINE_UNIVERSAL_OPERATOR(^)

#undef COMPILATION_TEST_DEFINE_UNIVERSAL_OPERATOR

}

TEST_CASE("ADL universal operators don't hijack expression deconstruction", "[compilation][.approvals]") {
    REQUIRE(adl::always_true{});
    REQUIRE(0 == adl::always_true{});
    REQUIRE(0 != adl::always_true{});
    REQUIRE(0 < adl::always_true{});
    REQUIRE(0 > adl::always_true{});
    REQUIRE(0 <= adl::always_true{});
    REQUIRE(0 >= adl::always_true{});
    REQUIRE(0 | adl::always_true{});
    REQUIRE(0 & adl::always_true{});
    REQUIRE(0 ^ adl::always_true{});
}

TEST_CASE( "#2555 - types that can only be compared with 0 literal (not int/long) are supported", "[compilation][approvals]" ) {
    REQUIRE( TypeWithLit0Comparisons{} < 0 );
    REQUIRE_FALSE( 0 < TypeWithLit0Comparisons{} );
    REQUIRE( TypeWithLit0Comparisons{} <= 0 );
    REQUIRE_FALSE( 0 > TypeWithLit0Comparisons{} );

    REQUIRE( TypeWithLit0Comparisons{} > 0 );
    REQUIRE_FALSE( 0 > TypeWithLit0Comparisons{} );
    REQUIRE( TypeWithLit0Comparisons{} >= 0 );
    REQUIRE_FALSE( 0 >= TypeWithLit0Comparisons{} );

    REQUIRE( TypeWithLit0Comparisons{} == 0 );
    REQUIRE_FALSE( 0 == TypeWithLit0Comparisons{} );
    REQUIRE( TypeWithLit0Comparisons{} != 0 );
    REQUIRE_FALSE( 0 != TypeWithLit0Comparisons{} );
}
