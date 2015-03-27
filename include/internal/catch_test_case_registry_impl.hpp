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

    class TestRegistry : public ITestCaseRegistry {
        struct LexSort {
            bool operator() (TestCase i,TestCase j) const { return (i<j);}
        };
        struct RandomNumberGenerator {
            int operator()( int n ) const { return std::rand() % n; }
        };

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

            if( m_functions.find( testCase ) == m_functions.end() ) {
                m_functions.insert( testCase );
                m_functionsInOrder.push_back( testCase );
                if( !testCase.isHidden() )
                    m_nonHiddenFunctions.push_back( testCase );
            }
            else {
                TestCase const& prev = *m_functions.find( testCase );
                {
                    Colour colourGuard( Colour::Red );
                    Catch::cerr()   << "error: TEST_CASE( \"" << name << "\" ) already defined.\n"
                                << "\tFirst seen at " << prev.getTestCaseInfo().lineInfo << "\n"
                                << "\tRedefined at " << testCase.getTestCaseInfo().lineInfo << std::endl;
                }
                exit(1);
            }
        }

        virtual std::vector<TestCase> const& getAllTests() const {
            return m_functionsInOrder;
        }

        virtual std::vector<TestCase> const& getAllNonHiddenTests() const {
            return m_nonHiddenFunctions;
        }

        virtual void getFilteredTests( TestSpec const& testSpec, IConfig const& config, std::vector<TestCase>& matchingTestCases, bool negated = false ) const {

            for( std::vector<TestCase>::const_iterator  it = m_functionsInOrder.begin(),
                                                        itEnd = m_functionsInOrder.end();
                    it != itEnd;
                    ++it ) {
                bool includeTest = testSpec.matches( *it ) && ( config.allowThrows() || !it->throws() );
                if( includeTest != negated )
                    matchingTestCases.push_back( *it );
            }
            sortTests( config, matchingTestCases );
        }

    private:

        static void sortTests( IConfig const& config, std::vector<TestCase>& matchingTestCases ) {
            
            switch( config.runOrder() ) {
                case RunTests::InLexicographicalOrder:
                    std::sort( matchingTestCases.begin(), matchingTestCases.end(), LexSort() );
                    break;
                case RunTests::InRandomOrder:
                {
                    RandomNumberGenerator rng;
                    std::random_shuffle( matchingTestCases.begin(), matchingTestCases.end(), rng );
                }
                    break;
                case RunTests::InDeclarationOrder:
                    // already in declaration order
                    break;
            }
        }
        std::set<TestCase> m_functions;
        std::vector<TestCase> m_functionsInOrder;
        std::vector<TestCase> m_nonHiddenFunctions;
        size_t m_unnamedCount;
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

    ///////////////////////////////////////////////////////////////////////////

    AutoReg::AutoReg(   TestFunction function,
                        SourceLineInfo const& lineInfo,
                        NameAndDesc const& nameAndDesc ) {
        registerTestCase( new FreeFunctionTestCase( function ), "", nameAndDesc, lineInfo );
    }

    AutoReg::~AutoReg() {}

    void AutoReg::registerTestCase( ITestCase* testCase,
                                    char const* classOrQualifiedMethodName,
                                    NameAndDesc const& nameAndDesc,
                                    SourceLineInfo const& lineInfo ) {

        getMutableRegistryHub().registerTest
            ( makeTestCase( testCase,
                            extractClassName( classOrQualifiedMethodName ),
                            nameAndDesc.name,
                            nameAndDesc.description,
                            lineInfo ) );
    }

} // end namespace Catch


#endif // TWOBLUECUBES_CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED
