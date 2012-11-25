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
#include "catch_test_spec.h"
#include "catch_context.h"

#include <vector>
#include <set>
#include <sstream>
#include <iostream>

namespace Catch {

    class TestRegistry : public ITestCaseRegistry {
    public:
        TestRegistry() : m_unnamedCount( 0 ) {}
        virtual ~TestRegistry();
        
        virtual void registerTest( const TestCase& testCase ) {
            std::string name = testCase.getTestCaseInfo().name;
            if( name == "" ) {
                std::ostringstream oss;
                oss << name << "unnamed/" << ++m_unnamedCount;
                return registerTest( testCase.withName( oss.str() ) );
            }

            if( m_functions.find( testCase ) == m_functions.end() ) {
                m_functions.insert( testCase );
                m_functionsInOrder.push_back( testCase );
                if( !testCase.isHidden() )
                    m_nonHiddenFunctions.push_back( testCase );
            }
            else {
                const TestCase& prev = *m_functions.find( testCase );
                std::cerr   << "error: TEST_CASE( \"" << name << "\" ) already defined.\n"
                            << "\tFirst seen at " << SourceLineInfo( prev.getTestCaseInfo().lineInfo ) << "\n"
                            << "\tRedefined at " << SourceLineInfo( testCase.getTestCaseInfo().lineInfo ) << std::endl;
                exit(1);
            }
        }
        
        virtual const std::vector<TestCase>& getAllTests() const {
            return m_functionsInOrder;
        }

        virtual const std::vector<TestCase>& getAllNonHiddenTests() const {
            return m_nonHiddenFunctions;
        }

        // !TBD deprecated
        virtual std::vector<TestCase> getMatchingTestCases( const std::string& rawTestSpec ) const {
            std::vector<TestCase> matchingTests;
            getMatchingTestCases( rawTestSpec, matchingTests );
            return matchingTests;
        }

        // !TBD deprecated
        virtual void getMatchingTestCases( const std::string& rawTestSpec, std::vector<TestCase>& matchingTestsOut ) const {
            TestCaseFilter filter( rawTestSpec );

            std::vector<TestCase>::const_iterator it = m_functionsInOrder.begin();
            std::vector<TestCase>::const_iterator itEnd = m_functionsInOrder.end();
            for(; it != itEnd; ++it ) {
                if( filter.shouldInclude( *it ) ) {
                    matchingTestsOut.push_back( *it );
                }
            }
        }
        virtual void getMatchingTestCases( const TestCaseFilters& filters, std::vector<TestCase>& matchingTestsOut ) const {
            std::vector<TestCase>::const_iterator it = m_functionsInOrder.begin();
            std::vector<TestCase>::const_iterator itEnd = m_functionsInOrder.end();
            // !TBD: replace with algorithm
            for(; it != itEnd; ++it )
                if( filters.shouldInclude( *it ) )
                    matchingTestsOut.push_back( *it );
        }

    private:
        
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

    inline std::string extractClassName( const std::string& classOrQualifiedMethodName ) {
        std::string className = classOrQualifiedMethodName;
        if( className[0] == '&' )
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
                        const char* name,
                        const char* description,
                        const SourceLineInfo& lineInfo ) {
        registerTestCase( new FreeFunctionTestCase( function ), "global", name, description, lineInfo );
    }    
    
    AutoReg::~AutoReg() {}
    
    void AutoReg::registerTestCase( ITestCase* testCase,
                                    const char* classOrQualifiedMethodName,
                                    const char* name,
                                    const char* description,
                                    const SourceLineInfo& lineInfo ) {
        
        getMutableRegistryHub().registerTest( makeTestCase( testCase, extractClassName( classOrQualifiedMethodName ), name, description, lineInfo ) );
    }
    
} // end namespace Catch


#endif // TWOBLUECUBES_CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED
