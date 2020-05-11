/*
 *  Created by Phil on 7/1/2011
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED

#include <catch2/internal/catch_test_registry.hpp>
#include <catch2/interfaces/catch_interfaces_config.hpp>

#include <vector>
#include <set>
#include <algorithm>

namespace Catch {

    class TestCaseHandle;
    struct IConfig;
    class TestSpec;

    std::vector<TestCaseHandle> sortTests( IConfig const& config, std::vector<TestCaseHandle> const& unsortedTestCases );

    bool isThrowSafe( TestCaseHandle const& testCase, IConfig const& config );
    bool matchTest( TestCaseHandle const& testCase, TestSpec const& testSpec, IConfig const& config );

    void enforceNoDuplicateTestCases( std::vector<TestCaseHandle> const& functions );

    std::vector<TestCaseHandle> filterTests( std::vector<TestCaseHandle> const& testCases, TestSpec const& testSpec, IConfig const& config );
    std::vector<TestCaseHandle> const& getAllTestCasesSorted( IConfig const& config );

    class TestRegistry : public ITestCaseRegistry {
    public:
        ~TestRegistry() override = default;

        virtual void registerTest( std::unique_ptr<TestCaseInfo> testInfo, std::unique_ptr<ITestInvoker> testInvoker );

        std::vector<std::unique_ptr<TestCaseInfo>> const& getAllInfos() const override;
        std::vector<TestCaseHandle> const& getAllTests() const override;
        std::vector<TestCaseHandle> const& getAllTestsSorted( IConfig const& config ) const override;

    private:
        std::vector<std::unique_ptr<TestCaseInfo>> m_infos;
        std::vector<std::unique_ptr<ITestInvoker>> m_invokers;
        std::vector<TestCaseHandle> m_handles;
        mutable RunTests::InWhatOrder m_currentSortOrder = RunTests::InDeclarationOrder;
        mutable std::vector<TestCaseHandle> m_sortedFunctions;
    };

    ///////////////////////////////////////////////////////////////////////////

    class TestInvokerAsFunction final : public ITestInvoker {
        using TestType = void(*)();
        TestType m_testAsFunction;
    public:
        TestInvokerAsFunction(TestType testAsFunction) noexcept:
            m_testAsFunction(testAsFunction) {}

        void invoke() const override;
    };


    std::string extractClassName( StringRef const& classOrQualifiedMethodName );

    ///////////////////////////////////////////////////////////////////////////


} // end namespace Catch


#endif // TWOBLUECUBES_CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED
