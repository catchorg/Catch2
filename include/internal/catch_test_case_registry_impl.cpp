/*
 *  Created by Martin on 25/07/2017
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_test_case_registry_impl.h"

#include "catch_context.h"
#include "catch_enforce.h"
#include "catch_interfaces_registry_hub.h"
#include "catch_random_number_generator.h"
#include "catch_run_context.h"
#include "catch_string_manip.h"
#include "catch_test_case_info.h"

#include <algorithm>
#include <sstream>

namespace Catch {

    namespace {
        struct TestHasher {
            explicit TestHasher(Catch::SimplePcg32& rng) {
                basis = rng();
                basis <<= 32;
                basis |= rng();
            }

            uint64_t basis;

            uint64_t operator()(TestCase const& t) const {
                // Modified FNV-1a hash
                static constexpr uint64_t prime = 1099511628211;
                uint64_t hash = basis;
                for (const char c : t.name) {
                    hash ^= c;
                    hash *= prime;
                }
                return hash;
            }
        };
    } // end unnamed namespace


    std::vector<TestCase> sortTests( IConfig const& config, std::vector<TestCase> const& unsortedTestCases ) {
        switch( config.runOrder() ) {
            case RunTests::InDeclarationOrder:
                // already in declaration order
                break;

            case RunTests::InLexicographicalOrder: {
                std::vector<TestCase> sorted = unsortedTestCases;
                std::sort( sorted.begin(), sorted.end() );
                return sorted;
            }

            case RunTests::InRandomOrder: {
                seedRng( config );
                TestHasher h( rng() );

                using hashedTest = std::pair<uint64_t, TestCase const*>;
                std::vector<hashedTest> indexed_tests;
                indexed_tests.reserve( unsortedTestCases.size() );

                for (auto const& testCase : unsortedTestCases) {
                    indexed_tests.emplace_back(h(testCase), &testCase);
                }

                std::sort(indexed_tests.begin(), indexed_tests.end(),
                          [](hashedTest const& lhs, hashedTest const& rhs) {
                          if (lhs.first == rhs.first) {
                              return lhs.second->name < rhs.second->name;
                          }
                          return lhs.first < rhs.first;
                });

                std::vector<TestCase> sorted;
                sorted.reserve( indexed_tests.size() );

                for (auto const& hashed : indexed_tests) {
                    sorted.emplace_back(*hashed.second);
                }

                return sorted;
            }
        }
        return unsortedTestCases;
    }

    bool isThrowSafe( TestCase const& testCase, IConfig const& config ) {
        return !testCase.throws() || config.allowThrows();
    }

    bool matchTest( TestCase const& testCase, TestSpec const& testSpec, IConfig const& config ) {
        return testSpec.matches( testCase ) && isThrowSafe( testCase, config );
    }

    void enforceNoDuplicateTestCases( std::vector<TestCase> const& functions ) {
        std::set<TestCase> seenFunctions;
        for( auto const& function : functions ) {
            auto prev = seenFunctions.insert( function );
            CATCH_ENFORCE( prev.second,
                    "error: TEST_CASE( \"" << function.name << "\" ) already defined.\n"
                    << "\tFirst seen at " << prev.first->getTestCaseInfo().lineInfo << "\n"
                    << "\tRedefined at " << function.getTestCaseInfo().lineInfo );
        }
    }

    std::vector<TestCase> filterTests( std::vector<TestCase> const& testCases, TestSpec const& testSpec, IConfig const& config ) {
        std::vector<TestCase> filtered;
        filtered.reserve( testCases.size() );
        for (auto const& testCase : testCases) {
            if ((!testSpec.hasFilters() && !testCase.isHidden()) ||
                (testSpec.hasFilters() && matchTest(testCase, testSpec, config))) {
                filtered.push_back(testCase);
            }
        }
        return filtered;
    }
    std::vector<TestCase> const& getAllTestCasesSorted( IConfig const& config ) {
        return getRegistryHub().getTestCaseRegistry().getAllTestsSorted( config );
    }

    void TestRegistry::registerTest( TestCase const& testCase ) {
        std::string name = testCase.getTestCaseInfo().name;
        if( name.empty() ) {
            ReusableStringStream rss;
            rss << "Anonymous test case " << ++m_unnamedCount;
            return registerTest( testCase.withName( rss.str() ) );
        }
        m_functions.push_back( testCase );
    }

    std::vector<TestCase> const& TestRegistry::getAllTests() const {
        return m_functions;
    }
    std::vector<TestCase> const& TestRegistry::getAllTestsSorted( IConfig const& config ) const {
        if( m_sortedFunctions.empty() )
            enforceNoDuplicateTestCases( m_functions );

        if(  m_currentSortOrder != config.runOrder() || m_sortedFunctions.empty() ) {
            m_sortedFunctions = sortTests( config, m_functions );
            m_currentSortOrder = config.runOrder();
        }
        return m_sortedFunctions;
    }



    ///////////////////////////////////////////////////////////////////////////
    TestInvokerAsFunction::TestInvokerAsFunction( void(*testAsFunction)() ) noexcept : m_testAsFunction( testAsFunction ) {}

    void TestInvokerAsFunction::invoke() const {
        m_testAsFunction();
    }

    std::string extractClassName( StringRef const& classOrQualifiedMethodName ) {
        std::string className(classOrQualifiedMethodName);
        if( startsWith( className, '&' ) )
        {
            std::size_t lastColons = className.rfind( "::" );
            std::size_t penultimateColons = className.rfind( "::", lastColons-1 );
            if( penultimateColons == std::string::npos )
                penultimateColons = 1;
            className = className.substr( penultimateColons, lastColons-penultimateColons );
        }
        return className;
    }

} // end namespace Catch
