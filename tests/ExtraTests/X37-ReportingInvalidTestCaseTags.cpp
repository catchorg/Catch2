
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

/**\file
 * Checks that TEST_CASE registration validates tags.
 */

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "Empty tag is not allowed", "[]" ) {}

TEST_CASE( "Exclamation point + unknown followup", "[!abc]" ) {}

TEST_CASE( "At is not allowed either (it is tag-alias in CLI)", "[@def]" ) {}
