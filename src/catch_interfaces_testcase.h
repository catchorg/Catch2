/*
 *  Created by Phil on 07/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_INTERFACES_TESTCASE_H_INCLUDED
#define TWOBLUECUBES_CATCH_INTERFACES_TESTCASE_H_INCLUDED

#include <vector>
#include <memory>

namespace Catch {

    class TestSpec;
    struct TestCaseInfo;

    struct ITestInvoker {
        virtual void invoke () const = 0;
        virtual ~ITestInvoker();
    };

    class TestCaseHandle;
    struct IConfig;

    struct ITestCaseRegistry {
        virtual ~ITestCaseRegistry();
        virtual std::vector<std::unique_ptr<TestCaseInfo>> const& getAllInfos() const = 0;
        virtual std::vector<TestCaseHandle> const& getAllTests() const = 0;
        virtual std::vector<TestCaseHandle> const& getAllTestsSorted( IConfig const& config ) const = 0;
    };

    bool isThrowSafe( TestCaseHandle const& testCase, IConfig const& config );
    bool matchTest( TestCaseHandle const& testCase, TestSpec const& testSpec, IConfig const& config );
    std::vector<TestCaseHandle> filterTests( std::vector<TestCaseHandle> const& testCases, TestSpec const& testSpec, IConfig const& config );
    std::vector<TestCaseHandle> const& getAllTestCasesSorted( IConfig const& config );

}

#endif // TWOBLUECUBES_CATCH_INTERFACES_TESTCASE_H_INCLUDED
