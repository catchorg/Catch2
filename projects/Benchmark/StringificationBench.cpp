/*
 *  Created by Martin on 16/01/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

#include <vector>

///////////////////////////////////////////////////////////////////////////////
TEST_CASE("Successful tests -- REQUIRE", "[Success]") {
    const std::size_t sz = 1 * 1024 * 1024;


    std::vector<std::size_t> vec; vec.reserve(sz);
    for (std::size_t i = 0; i < sz; ++i){
        vec.push_back(i);
        REQUIRE(vec.back() == i);
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE("Successful tests -- CHECK", "[Success]") {
    const std::size_t sz = 1 * 1024 * 1024;


    std::vector<std::size_t> vec; vec.reserve(sz);
    for (std::size_t i = 0; i < sz; ++i){
        vec.push_back(i);
        CHECK(vec.back() == i);
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE("Unsuccessful tests -- CHECK", "[Failure]") {
    const std::size_t sz = 1024 * 1024;


    std::vector<std::size_t> vec; vec.reserve(sz);
    for (std::size_t i = 0; i < sz; ++i){
        vec.push_back(i);
        CHECK(vec.size() == i);
    }
}
