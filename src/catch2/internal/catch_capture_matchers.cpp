/*
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include <catch2/internal/catch_capture_matchers.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/interfaces/catch_interfaces_registry_hub.hpp>

namespace Catch {

    // This is the general overload that takes a any string matcher
    // There is another overload, in catch_assertionhandler.h/.cpp, that only takes a string and infers
    // the Equals matcher (so the header does not mention matchers)
    void handleExceptionMatchExpr( AssertionHandler& handler, StringMatcher const& matcher, StringRef const& matcherString  ) {
        std::string exceptionMessage = Catch::translateActiveException();
        MatchExpr<std::string, StringMatcher const&> expr( std::move(exceptionMessage), matcher, matcherString );
        handler.handleExpr( expr );
    }

} // namespace Catch
