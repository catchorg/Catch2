/*
 *  Created by Martin on 17/02/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER
#include "catch.hpp"

#include <map>
#include <set>

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

TEST_CASE( "std::map is convertible string", "[toString]" ) {

    SECTION( "empty" ) {
        std::map<std::string, int> emptyMap;

        REQUIRE( Catch::Detail::stringify( emptyMap ) == "{  }" );
    }

    SECTION( "single item" ) {
        std::map<std::string, int> map = { { "one", 1 } };

        REQUIRE( Catch::Detail::stringify( map ) == "{ { \"one\", 1 } }" );
    }

    SECTION( "several items" ) {
        std::map<std::string, int> map = {
                { "abc", 1 },
                { "def", 2 },
                { "ghi", 3 }
            };

        REQUIRE( Catch::Detail::stringify( map ) == "{ { \"abc\", 1 }, { \"def\", 2 }, { \"ghi\", 3 } }" );
    }
}

TEST_CASE( "std::set is convertible string", "[toString]" ) {

    SECTION( "empty" ) {
        std::set<std::string> emptySet;

        REQUIRE( Catch::Detail::stringify( emptySet ) == "{  }" );
    }

    SECTION( "single item" ) {
        std::set<std::string> set = { "one" };

        REQUIRE( Catch::Detail::stringify( set ) == "{ \"one\" }" );
    }

    SECTION( "several items" ) {
        std::set<std::string> set = { "abc", "def", "ghi" };

        REQUIRE( Catch::Detail::stringify( set ) == "{ \"abc\", \"def\", \"ghi\" }" );
    }
}

TEST_CASE("Static arrays are convertible to string", "[toString]") {
    SECTION("Single item") {
        int singular[1] = { 1 };
        REQUIRE(Catch::Detail::stringify(singular) == "{ 1 }");
    }
    SECTION("Multiple") {
        int arr[3] = { 3, 2, 1 };
        REQUIRE(Catch::Detail::stringify(arr) == "{ 3, 2, 1 }");
    }
    SECTION("Non-trivial inner items") {
        std::vector<std::string> arr[2] = { {"1:1", "1:2", "1:3"}, {"2:1", "2:2"} };
        REQUIRE(Catch::Detail::stringify(arr) == R"({ { "1:1", "1:2", "1:3" }, { "2:1", "2:2" } })");
    }
}
