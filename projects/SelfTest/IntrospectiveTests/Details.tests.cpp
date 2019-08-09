/*
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4702) // unreachable code in the macro expansions
#endif

TEST_CASE("Check that our error handling macros throw the right exceptions", "[!throws][internals][approvals]") {
    REQUIRE_THROWS_AS(CATCH_INTERNAL_ERROR(""), std::logic_error);
    REQUIRE_THROWS_AS(CATCH_ERROR(""), std::domain_error);
    REQUIRE_THROWS_AS(CATCH_RUNTIME_ERROR(""), std::runtime_error);
    REQUIRE_THROWS_AS([](){CATCH_ENFORCE(false, "");}(), std::domain_error);
    REQUIRE_NOTHROW([](){CATCH_ENFORCE(true, "");}());
}

#if defined(_MSC_VER)
#pragma warning(pop) // unreachable code in the macro expansions
#endif
