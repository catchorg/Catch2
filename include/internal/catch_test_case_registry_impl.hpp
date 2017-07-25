/*
 *  Created by Phil on 7/1/2011
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED

#include "catch_test_registry.hpp"
#include "catch_test_spec.hpp"
#include "catch_interfaces_config.h"

#include <vector>
#include <set>
#include <algorithm>


namespace Catch {

    class TestCase;
    class IConfig;

    struct RandomNumberGenerator {
        using result_type = std::ptrdiff_t;

        result_type operator()( result_type n ) const;

        static constexpr result_type min() { return 0; }
        static constexpr result_type max() { return 1000000; }
        result_type operator()() const;
        template<typename V>
        static void shuffle( V& vector ) {
            RandomNumberGenerator rng;
            std::shuffle( vector.begin(), vector.end(), rng );
        }
    };

    std::vector<TestCase> sortTests( IConfig const& config, std::vector<TestCase> const& unsortedTestCases ); 
    bool matchTest( TestCase const& testCase, TestSpec const& testSpec, IConfig const& config );

    void enforceNoDuplicateTestCases( std::vector<TestCase> const& functions );

    std::vector<TestCase> filterTests( std::vector<TestCase> const& testCases, TestSpec const& testSpec, IConfig const& config );
    std::vector<TestCase> const& getAllTestCasesSorted( IConfig const& config );

    class TestRegistry : public ITestCaseRegistry {
    public:
        virtual ~TestRegistry() = default;

        virtual void registerTest( TestCase const& testCase );

        std::vector<TestCase> const& getAllTests() const override;
        std::vector<TestCase> const& getAllTestsSorted( IConfig const& config ) const override;

    private:
        std::vector<TestCase> m_functions;
        mutable RunTests::InWhatOrder m_currentSortOrder = RunTests::InDeclarationOrder;
        mutable std::vector<TestCase> m_sortedFunctions;
        size_t m_unnamedCount = 0;
        std::ios_base::Init m_ostreamInit; // Forces cout/ cerr to be initialised
    };

    ///////////////////////////////////////////////////////////////////////////

    class TestInvokerAsFunction : public ITestInvoker {
        void(*m_testAsFunction)();
    public:
        TestInvokerAsFunction( void(*testAsFunction)() ) noexcept;

        void invoke() const override;
    };


    std::string extractClassName( std::string const& classOrQualifiedMethodName );

    ///////////////////////////////////////////////////////////////////////////


} // end namespace Catch


#endif // TWOBLUECUBES_CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED
