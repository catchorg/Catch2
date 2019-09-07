#include "internal/catch_string_manip.h"

#include "catch.hpp"

static const char * const no_whitespace = "There is no extra whitespace here";
static const char * const leading_whitespace = " \r \t\n There is no extra whitespace here";
static const char * const trailing_whitespace = "There is no extra whitespace here \t \n \r ";
static const char * const whitespace_at_both_ends = " \r\n \t There is no extra whitespace here  \t\t\t \n";

TEST_CASE("Trim strings", "[string-manip]") {
    using Catch::trim; using Catch::StringRef;
    static_assert(std::is_same<std::string, decltype(trim(std::string{}))>::value, "Trimming std::string should return std::string");
    static_assert(std::is_same<StringRef, decltype(trim(StringRef{}))>::value, "Trimming StringRef should return StringRef");

    REQUIRE(trim(std::string(no_whitespace)) == no_whitespace);
    REQUIRE(trim(std::string(leading_whitespace)) == no_whitespace);
    REQUIRE(trim(std::string(trailing_whitespace)) == no_whitespace);
    REQUIRE(trim(std::string(whitespace_at_both_ends)) == no_whitespace);

    REQUIRE(trim(StringRef(no_whitespace)) == StringRef(no_whitespace));
    REQUIRE(trim(StringRef(leading_whitespace)) == StringRef(no_whitespace));
    REQUIRE(trim(StringRef(trailing_whitespace)) == StringRef(no_whitespace));
    REQUIRE(trim(StringRef(whitespace_at_both_ends)) == StringRef(no_whitespace));
}
