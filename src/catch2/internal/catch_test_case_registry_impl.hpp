
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED
#define CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED

#include <catch2/internal/catch_unique_ptr.hpp>

#include <vector>

namespace Catch {

    class TestCaseHandle;
    class IConfig;
    class TestSpec;
    class ITestInvoker;
    struct TestCaseInfo;

    std::vector<TestCaseHandle> sortTests( IConfig const& config, std::vector<TestCaseHandle> const& unsortedTestCases );

    bool isThrowSafe( TestCaseHandle const& testCase, IConfig const& config );

    std::vector<TestCaseHandle> filterTests( std::vector<TestCaseHandle> const& testCases, TestSpec const& testSpec, IConfig const& config );
    std::vector<TestCaseHandle> const& getAllTestCasesSorted( IConfig const& config );

    class TestCaseRegistry {
        struct TestCaseRegistryImpl;
        Detail::unique_ptr<TestCaseRegistryImpl> m_impl;
    public:
        TestCaseRegistry();
        ~TestCaseRegistry(); // = default;

        void registerTest( Detail::unique_ptr<TestCaseInfo> testInfo, Detail::unique_ptr<ITestInvoker> testInvoker );

        std::vector<TestCaseInfo*> const& getAllInfos() const;
        std::vector<TestCaseHandle> const& getAllTests() const;
        std::vector<TestCaseHandle> const& getAllTestsSorted( IConfig const& config ) const;
    };

    ///////////////////////////////////////////////////////////////////////////


} // end namespace Catch


#endif // CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED
