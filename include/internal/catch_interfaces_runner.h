/*
 *  Created by Phil on 07/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_INTERNAL_CATCH_INTERFACES_RUNNER_H_INCLUDED
#define TWOBLUECUBES_INTERNAL_CATCH_INTERFACES_RUNNER_H_INCLUDED

#include "catch_totals.hpp"

#include <string>

namespace Catch {
    class TestCaseInfo;
    
    struct IRunner {
        virtual ~IRunner();

        /// Runs all tests, even if hidden
        virtual Totals runAll() = 0;

        /// Runs all tests unless 'hidden' by ./ prefix
        virtual Totals runAllNonHidden() = 0;

        /// Runs all test that match the spec string
        virtual Totals runMatching( const std::string& rawTestSpec ) = 0;

        /// Runs all the tests passed in
        virtual Totals runTests( const std::string& groupName, const std::vector<TestCaseInfo>& testCases ) = 0;
    };
}

#endif // TWOBLUECUBES_INTERNAL_CATCH_INTERFACES_RUNNER_H_INCLUDED
