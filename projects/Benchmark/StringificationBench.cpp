#include "catch.hpp"

#include <vector>

///////////////////////////////////////////////////////////////////////////////
TEST_CASE("Successful tests -- REQUIRE", "[Success]") {
    const size_t sz = 1 * 1024 * 1024;


    std::vector<size_t> vec; vec.reserve(sz);
    for (size_t i = 0; i < sz; ++i){
        vec.push_back(i);
        REQUIRE(vec.back() == i);
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE("Successful tests -- CHECK", "[Success]") {
    const size_t sz = 1 * 1024 * 1024;


    std::vector<size_t> vec; vec.reserve(sz);
    for (size_t i = 0; i < sz; ++i){
        vec.push_back(i);
        CHECK(vec.back() == i);
    }
}

///////////////////////////////////////////////////////////////////////////////
TEST_CASE("Unsuccessful tests -- CHECK", "[Failure]") {
    const size_t sz = 128 * 1024;


    std::vector<size_t> vec; vec.reserve(sz);
    for (size_t i = 0; i < sz; ++i){
        vec.push_back(i);
        CHECK(vec.size() == i);
    }
}
