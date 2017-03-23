/*
 *  Created by Martin on 17/02/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"


// This is a minimal example for an issue we have found in 1.7.0
struct foo {
    int i;
};

template <typename T>
bool operator==(const T& val, foo f){
    return val == f.i;
}

TEST_CASE("#809") {
    foo f; f.i = 42;
    REQUIRE(42 == f);
}


// ------------------------------------------------------------------
// Changes to REQUIRE_THROWS_AS made it stop working in a template in
// an unfixable way (as long as C++03 compatibility is being kept).
// To prevent these from happening in the future, this needs to compile

void throws_int(bool b) {
    if (b) {
        throw 1;
    }
}

template <typename T>
bool templated_tests(T t) {
    int a = 3;
    REQUIRE(a == t);
    CHECK(a == t);
    REQUIRE_THROWS(throws_int(true));
    CHECK_THROWS_AS(throws_int(true), const int&);
    REQUIRE_NOTHROW(throws_int(false));
    REQUIRE_THAT("aaa", Catch::EndsWith("aaa"));
    return true;
}

TEST_CASE("#833") {
    REQUIRE(templated_tests<int>(3));
}

