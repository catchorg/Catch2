//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
    
#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_clara.hpp>


#include <string>

TEST_CASE("Clara::Arg supports single-arg parse the way Opt does", "[clara][arg][compilation]") {
    std::string name;
    auto p = Catch::Clara::Arg(name, "just one arg");
    
    CHECK(name.empty());
    
    p.parse( Catch::Clara::Args{ "UnitTest", "foo" } );
    REQUIRE(name == "foo");
}
