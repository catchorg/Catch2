// X10-FallbackStringifier.cpp
// Test that defining fallbackStringifier compiles

#include <string>

// A catch-all stringifier
template <typename T>
std::string fallbackStringifier(T const&) {
    return "{ !!! }";
}

#include <catch2/catch_test_macros.hpp>

struct foo {
    explicit operator bool() const {
        return true;
    }
};

TEST_CASE("aa") {
    REQUIRE(foo{});
}
