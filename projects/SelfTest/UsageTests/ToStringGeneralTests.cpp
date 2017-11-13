/*
 *  Created by Martin on 17/02/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"


TEST_CASE( "Character pretty printing" ){
    SECTION("Specifically escaped"){
        char tab = '\t';
        char newline = '\n';
        char carr_return = '\r';
        char form_feed = '\f';
        CHECK(tab == '\t');
        CHECK(newline == '\n');
        CHECK(carr_return == '\r');
        CHECK(form_feed == '\f');
    }
    SECTION("General chars"){
        char space = ' ';
        CHECK(space == ' ');
        char chars[] = {'a', 'z', 'A', 'Z'};
        for (int i = 0; i < 4; ++i){
            char c = chars[i];
            REQUIRE(c == chars[i]);
        }
    }
    SECTION("Low ASCII"){
        char null_terminator = '\0';
        CHECK(null_terminator == '\0');
        for (int i = 2; i < 6; ++i){
            char c = static_cast<char>(i);
            REQUIRE(c == i);
        }
    }
}


TEST_CASE( "Capture and info messages" ) {
    SECTION("Capture should stringify like assertions") {
        int i = 2;
        CAPTURE(i);
        REQUIRE(true);
    }
    SECTION("Info should NOT stringify the way assertions do") {
        int i = 3;
        INFO(i);
        REQUIRE(true);
    }
}
