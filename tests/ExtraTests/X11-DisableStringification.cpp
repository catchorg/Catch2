// X11-DisableStringification.cpp
// Test that stringification of original expression can be disabled
// this is a workaround for VS 2017 issue with Raw String literal
// and preprocessor token pasting. In other words, hopefully this test
// will be deleted soon :-)

#include <catch2/catch_test_macros.hpp>

namespace {
    struct Hidden {};

    bool operator==(Hidden, Hidden) { return true; }
}

TEST_CASE("DisableStringification") {
    REQUIRE( Hidden{} == Hidden{} );
}
