
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_test_case_registry_impl.hpp>

#include <catch2/internal/catch_enforce.hpp>
#include <catch2/interfaces/catch_interfaces_config.hpp>
#include <catch2/interfaces/catch_interfaces_registry_hub.hpp>
#include <catch2/internal/catch_sharding.hpp>
#include <catch2/catch_test_case_info.hpp>
#include <catch2/catch_test_spec.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>
#include <catch2/internal/catch_test_case_info_hasher.hpp>

#include <algorithm>

namespace Catch {

    namespace {
        // Picked small-ish number at random
        static size_t kInitialTestCount = 120;

        static void enforceNoDuplicateTestCases(
            std::vector<TestCaseHandle> const& tests ) {
            // Flat set instead of std::set for performance.
            std::vector<TestCaseInfo const*> seenTests;
            seenTests.reserve( tests.size() );
            for ( auto const& test : tests ) {
                seenTests.push_back( &test.getTestCaseInfo() );
            }
            // We want to keep order of ~equal~ tests
            // stable_sort so that duplicates are reported with the
            // registration order preserved ("first seen" really is first)
            std::stable_sort(
                seenTests.begin(),
                seenTests.end(),
                []( TestCaseInfo const* lhs, TestCaseInfo const* rhs ) {
                    return *lhs < *rhs;
                }
            );
            const auto duplicate = std::adjacent_find(
                seenTests.begin(),
                seenTests.end(),
                []( TestCaseInfo const* lhs, TestCaseInfo const* rhs ) {
                    return *lhs == *rhs;
                }
            );

            if ( duplicate != seenTests.end() ) {
                const auto infoPtr = *std::next( duplicate );
                CATCH_ERROR( "error: test case \""
                             << infoPtr->name << "\", with tags \""
                             << infoPtr->tagsAsString()
                             << "\" already defined.\n"
                             << "\tFirst seen at "
                             << ( *duplicate )->lineInfo << "\n"
                             << "\tRedefined at " << infoPtr->lineInfo );
            }
        }

        static bool matchTest( TestCaseHandle const& testCase,
                               TestSpec const& testSpec,
                               IConfig const& config ) {
            return testSpec.matches( testCase.getTestCaseInfo() ) &&
                   isThrowSafe( testCase, config );
        }

    } // end unnamed namespace

    std::vector<TestCaseHandle> sortTests( IConfig const& config, std::vector<TestCaseHandle> const& unsortedTestCases ) {
        switch (config.runOrder()) {
        case TestRunOrder::Declared:
            return unsortedTestCases;

        case TestRunOrder::LexicographicallySorted: {
            std::vector<TestCaseHandle> sorted = unsortedTestCases;
            std::sort(
                sorted.begin(),
                sorted.end(),
                []( TestCaseHandle const& lhs, TestCaseHandle const& rhs ) {
                    return lhs.getTestCaseInfo() < rhs.getTestCaseInfo();
                }
            );
            return sorted;
        }
        case TestRunOrder::Randomized: {
            using TestWithHash = std::pair<TestCaseInfoHasher::hash_t, TestCaseHandle>;

            TestCaseInfoHasher h{ config.rngSeed() };
            std::vector<TestWithHash> indexed_tests;
            indexed_tests.reserve(unsortedTestCases.size());

            for (auto const& handle : unsortedTestCases) {
                indexed_tests.emplace_back(h(handle.getTestCaseInfo()), handle);
            }

            std::sort( indexed_tests.begin(),
                       indexed_tests.end(),
                       []( TestWithHash const& lhs, TestWithHash const& rhs ) {
                           if ( lhs.first == rhs.first ) {
                               return lhs.second.getTestCaseInfo() <
                                      rhs.second.getTestCaseInfo();
                           }
                           return lhs.first < rhs.first;
                       } );

            std::vector<TestCaseHandle> randomized;
            randomized.reserve(indexed_tests.size());

            for (auto const& indexed : indexed_tests) {
                randomized.push_back(indexed.second);
            }

            return randomized;
        }
        }

        CATCH_INTERNAL_ERROR("Unknown test order value!");
    }

    bool isThrowSafe( TestCaseHandle const& testCase, IConfig const& config ) {
        return !testCase.getTestCaseInfo().throws() || config.allowThrows();
    }

    std::vector<TestCaseHandle> filterTests( std::vector<TestCaseHandle> const& testCases, TestSpec const& testSpec, IConfig const& config ) {
        std::vector<TestCaseHandle> filtered;
        filtered.reserve( testCases.size() );
        for (auto const& testCase : testCases) {
            if ((!testSpec.hasFilters() && !testCase.getTestCaseInfo().isHidden()) ||
                (testSpec.hasFilters() && matchTest(testCase, testSpec, config))) {
                filtered.push_back(testCase);
            }
        }
        return createShard(filtered, config.shardCount(), config.shardIndex());
    }
    std::vector<TestCaseHandle> const& getAllTestCasesSorted( IConfig const& config ) {
        return getRegistryHub().getTestCaseRegistry().getAllTestsSorted( config );
    }


    TestRegistry::TestRegistry() {
        // We pre-reserve some reasonable number of tests to avoid the
        // initial geometric growth churning during test registration.
        m_handles.reserve( kInitialTestCount );
        m_test_infos.reserve( kInitialTestCount );
        m_invokers.reserve( kInitialTestCount );
    }
    TestRegistry::~TestRegistry() = default;

    void TestRegistry::registerTest(Detail::unique_ptr<TestCaseInfo> testInfo, Detail::unique_ptr<ITestInvoker> testInvoker) {
        m_handles.emplace_back(testInfo.get(), testInvoker.get());
        m_test_infos.push_back(CATCH_MOVE(testInfo));
        m_invokers.push_back(CATCH_MOVE(testInvoker));
    }

    void TestRegistry::enableFilenameTags() {
        for (auto& info : m_test_infos) {
            info->addFilenameTag();
        }
    }

    std::vector<TestCaseHandle> const& TestRegistry::getAllTests() const {
        return m_handles;
    }
    std::vector<TestCaseHandle> const& TestRegistry::getAllTestsSorted( IConfig const& config ) const {
        if( m_sortedFunctions.empty() )
            enforceNoDuplicateTestCases( m_handles );

        if(  m_currentSortOrder != config.runOrder() || m_sortedFunctions.empty() ) {
            m_sortedFunctions = sortTests( config, m_handles );
            m_currentSortOrder = config.runOrder();
        }
        return m_sortedFunctions;
    }

} // end namespace Catch
