
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <iostream>

namespace {

struct PrintsWhenConstructed {
    PrintsWhenConstructed() {
        std::cout << "Hello\n";
        std::cerr << "Holla\n";
        std::fprintf(stdout, "Hullo\n");
        std::fprintf(stderr, "Hillo\n");
    }
};

static PrintsWhenConstructed instance;

}

TEST_CASE("@Script[C:\\EPM1A]=x;\"SCALA_ZERO:\"", "[script regressions]"){}
TEST_CASE("Some test") {}
TEST_CASE( "Let's have a test case with a long name. Longer. No, even longer. "
           "Really looooooooooooong. Even longer than that. Multiple lines "
           "worth of test name. Yep, like this." ) {}
TEST_CASE( "And now a test case with weird tags.", "[tl;dr][tl;dw][foo,bar]" ) {}
// Also check that we handle tests on class, which have name in output as 'class-name', not 'name'.
class TestCaseFixture {
public:
    int m_a;
};

TEST_CASE_METHOD(TestCaseFixture, "A test case as method", "[tagstagstags]") {}

TEST_CASE("Unclosed right ) parenthesis") {}
TEST_CASE("Unclosed left ( parenthesis") {}

TEST_CASE( "Newlines\nAnd\rOther\n\tWhitespace", "[whitespace-going-wild]" ) {}
TEST_CASE( "Escaped \\n newline and \\r other whitespace", "[whitespace-going-wild]") {}

// Some JSON-like and JSON-adjacent characters and substrings in the test names/tags
// This serves to test that the parse-json-via-string-splitting hack in
// catch_discover_tests works properly.
TEST_CASE( "We split on variants of },{ in name" ) {}
TEST_CASE( "Then }\t, { we }\t,\t{ concatenate } , { them } ,{back},{" ) {}
TEST_CASE( "}},{{" ) {}
TEST_CASE( "Arrays [{},{}] wheee", "[also},{tags]" ) {}
TEST_CASE( "Let's add semicolon into the mix ;},{;},{};,{}" ) {}
TEST_CASE( "[", "[unmatched-square-bracket]" ) {}
TEST_CASE( "]", "[unmatched-square-bracket]" ) {}

// Some CMake-like special strings ($ as dereference) strings in test names.
// This serves to test that the names of test cases are not evaluated
// inside the catch_discover_tests.
TEST_CASE( "Plain ${NOT_A_VAR} variable" ) {}
TEST_CASE( "Env variable access $ENV{HOME}" ) {}
TEST_CASE( "Cache check $CACHE{FOO}" ) {}
TEST_CASE( "Also some generator exprs $<CONFIG> in $<1:yes> name" ) {}
TEST_CASE( "Mess of bare $ $$ $$$ and unterminated $} ${ exprs" ) {}
TEST_CASE( "$ENV{X};[weird]{},{ mix $<0:no> ${VAR} };,{ }},{{" ) {}
