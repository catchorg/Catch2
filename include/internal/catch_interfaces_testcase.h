/*
 *  Created by Phil on 07/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_INTERFACES_TESTCASE_H_INCLUDED
#define TWOBLUECUBES_CATCH_INTERFACES_TESTCASE_H_INCLUDED

#include "catch_ptr.hpp"

#include <vector>

namespace Catch {

    class TestCaseFilters;

    struct ITestCase : IShared {
        virtual void invoke () const = 0;
    protected:
        virtual ~ITestCase();
    };

    class TestCase;
    struct IConfig;

    struct ITestCaseRegistry {
        virtual ~ITestCaseRegistry();
        virtual std::vector<TestCase> const& getAllTests() const = 0;
        virtual void getFilteredTests( TestCaseFilters const& filters, IConfig const& config, std::vector<TestCase>& matchingTestCases ) const = 0;
        virtual void getFilteredTests( IConfig const& config, std::vector<TestCase>& matchingTestCases ) const = 0;

    };
}

#endif // TWOBLUECUBES_CATCH_INTERFACES_TESTCASE_H_INCLUDED
