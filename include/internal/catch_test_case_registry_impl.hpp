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
#include "catch_test_case_info.h"
#include "catch_test_spec.hpp"
#include "catch_context.h"

#include <vector>
#include <set>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace Catch {

    struct LexSort {
        bool operator() (TestCase i,TestCase j) const { return (i<j);}
    };
    struct RandomNumberGenerator {
        int operator()( int n ) const { return std::rand() % n; }
    };

    inline std::vector<TestCase> sortTests( IConfig const& config, std::vector<TestCase> const& unsortedTestCases ) {

        std::vector<TestCase> sorted = unsortedTestCases;

        switch( config.runOrder() ) {
            case RunTests::InLexicographicalOrder:
                std::sort( sorted.begin(), sorted.end(), LexSort() );
                break;
            case RunTests::InRandomOrder:
                {
                    seedRng( config );

                    RandomNumberGenerator rng;
                    std::random_shuffle( sorted.begin(), sorted.end(), rng );
                }
                break;
            case RunTests::InDeclarationOrder:
                // already in declaration order
                break;
        }
        return sorted;
    }
    bool matchTest( TestCase const& testCase, TestSpec const& testSpec, IConfig const& config ) {
        return testSpec.matches( testCase ) && ( config.allowThrows() || !testCase.throws() );
    }

    void enforceNoDuplicateTestCases( std::vector<TestCase> const& functions ) {
        std::set<TestCase> seenFunctions;
        for( std::vector<TestCase>::const_iterator it = functions.begin(), itEnd = functions.end();
            it != itEnd;
            ++it ) {
            std::pair<std::set<TestCase>::const_iterator, bool> prev = seenFunctions.insert( *it );
            if( !prev.second ){
                Catch::cerr()
                << Colour( Colour::Red )
                << "error: TEST_CASE( \"" << it->name << "\" ) already defined.\n"
                << "\tFirst seen at " << prev.first->getTestCaseInfo().lineInfo << "\n"
                << "\tRedefined at " << it->getTestCaseInfo().lineInfo << std::endl;
                exit(1);
            }
        }
    }

    std::vector<TestCase> filterTests( std::vector<TestCase> const& testCases, TestSpec const& testSpec, IConfig const& config ) {
        std::vector<TestCase> filtered;
        filtered.reserve( testCases.size() );
        for( std::vector<TestCase>::const_iterator it = testCases.begin(), itEnd = testCases.end();
                it != itEnd;
                ++it )
            if( matchTest( *it, testSpec, config ) )
                filtered.push_back( *it );
        return filtered;
    }
    std::vector<TestCase> const& getAllTestCasesSorted( IConfig const& config ) {
        return getRegistryHub().getTestCaseRegistry().getAllTestsSorted( config );
    }

    class TestRegistry : public ITestCaseRegistry {
    public:
        TestRegistry() : m_unnamedCount( 0 ) {}
        virtual ~TestRegistry();

        virtual void registerTest( TestCase const& testCase ) {
            std::string name = testCase.getTestCaseInfo().name;
            if( name == "" ) {
                std::ostringstream oss;
                oss << "Anonymous test case " << ++m_unnamedCount;
                return registerTest( testCase.withName( oss.str() ) );
            }
            m_functions.push_back( testCase );
        }

        virtual std::vector<TestCase> const& getAllTests() const {
            return m_functions;
        }
        virtual std::vector<TestCase> const& getAllTestsSorted( IConfig const& config ) const {
            if( m_sortedFunctions.empty() )
                enforceNoDuplicateTestCases( m_functions );

            if(  m_currentSortOrder != config.runOrder() || m_sortedFunctions.empty() ) {
                m_sortedFunctions = sortTests( config, m_functions );
                m_currentSortOrder = config.runOrder();
            }
            return m_sortedFunctions;
        }

    private:
        std::vector<TestCase> m_functions;
        mutable RunTests::InWhatOrder m_currentSortOrder;
        mutable std::vector<TestCase> m_sortedFunctions;
        size_t m_unnamedCount;
        std::ios_base::Init m_ostreamInit; // Forces cout/ cerr to be initialised
    };

    ///////////////////////////////////////////////////////////////////////////

    class FreeFunctionTestCase : public SharedImpl<ITestCase> {
    public:

        FreeFunctionTestCase( TestFunction fun ) : m_fun( fun ) {}

        virtual void invoke() const {
            m_fun();
        }

    private:
        virtual ~FreeFunctionTestCase();

        TestFunction m_fun;
    };

    inline std::string extractClassName( std::string const& classOrQualifiedMethodName ) {
        std::string className = classOrQualifiedMethodName;
        if( startsWith( className, "&" ) )
        {
            std::size_t lastColons = className.rfind( "::" );
            std::size_t penultimateColons = className.rfind( "::", lastColons-1 );
            if( penultimateColons == std::string::npos )
                penultimateColons = 1;
            className = className.substr( penultimateColons, lastColons-penultimateColons );
        }
        return className;
    }

    void registerTestCase
        (   ITestCase* testCase,
            char const* classOrQualifiedMethodName,
            NameAndDesc const& nameAndDesc,
            SourceLineInfo const& lineInfo ) {
        
        getMutableRegistryHub().registerTest
            ( makeTestCase
                (   testCase,
                    extractClassName( classOrQualifiedMethodName ),
                    nameAndDesc.name,
                    nameAndDesc.description,
                    lineInfo ) );
    }
    void registerTestCaseFunction
        (   TestFunction function,
            SourceLineInfo const& lineInfo,
            NameAndDesc const& nameAndDesc ) {
        registerTestCase( new FreeFunctionTestCase( function ), "", nameAndDesc, lineInfo );
    }
    
    ///////////////////////////////////////////////////////////////////////////

    AutoReg::AutoReg
        (   TestFunction function,
            SourceLineInfo const& lineInfo,
            NameAndDesc const& nameAndDesc ) {
        registerTestCaseFunction( function, lineInfo, nameAndDesc );
    }

    AutoReg::~AutoReg() {}

} // end namespace Catch


#endif // TWOBLUECUBES_CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED
