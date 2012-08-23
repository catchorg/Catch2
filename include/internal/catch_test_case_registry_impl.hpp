/*
 *  Created by Phil on 7/1/2011
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
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
        
        virtual void registerTest( const TestCaseInfo& testInfo ) {
            if( testInfo.getName() == "" ) {
                std::ostringstream oss;
                oss << testInfo.getName() << "unnamed/" << ++m_unnamedCount;
                return registerTest( TestCaseInfo( testInfo, oss.str() ) );
            }

            if( m_functions.find( testInfo ) == m_functions.end() ) {
                m_functions.insert( testInfo );
                m_functionsInOrder.push_back( testInfo );
                if( !testInfo.isHidden() )
                    m_nonHiddenFunctions.push_back( testInfo );
            }
            else {
                const TestCaseInfo& prev = *m_functions.find( testInfo );
                std::cerr   << "error: TEST_CASE( \"" << testInfo.getName() << "\" ) already defined.\n"
                            << "\tFirst seen at " << SourceLineInfo( prev.getLineInfo() ) << "\n"
                            << "\tRedefined at " << SourceLineInfo( testInfo.getLineInfo() ) << std::endl;
                exit(1);
            }
        }
        
        virtual const std::vector<TestCaseInfo>& getAllTests() const {
            return m_functionsInOrder;
        }

        virtual const std::vector<TestCaseInfo>& getAllNonHiddenTests() const {
            return m_nonHiddenFunctions;
        }

        // !TBD deprecated
        virtual std::vector<TestCaseInfo> getMatchingTestCases( const std::string& rawTestSpec ) const {
            std::vector<TestCaseInfo> matchingTests;
            getMatchingTestCases( rawTestSpec, matchingTests );
            return matchingTests;
        }

        // !TBD deprecated
        virtual void getMatchingTestCases( const std::string& rawTestSpec, std::vector<TestCaseInfo>& matchingTestsOut ) const {
            TestCaseFilter filter( rawTestSpec );

            std::vector<TestCaseInfo>::const_iterator it = m_functionsInOrder.begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = m_functionsInOrder.end();
            for(; it != itEnd; ++it ) {
                if( filter.shouldInclude( *it ) ) {
                    matchingTestsOut.push_back( *it );
                }
            }
        }
        virtual void getMatchingTestCases( const TestCaseFilters& filters, std::vector<TestCaseInfo>& matchingTestsOut ) const {
            std::vector<TestCaseInfo>::const_iterator it = m_functionsInOrder.begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = m_functionsInOrder.end();
            // !TBD: replace with algorithm
            for(; it != itEnd; ++it )
                if( filters.shouldInclude( *it ) )
                    matchingTestsOut.push_back( *it );
        }

    private:
        
        std::set<TestCaseInfo> m_functions;
        std::vector<TestCaseInfo> m_functionsInOrder;
        std::vector<TestCaseInfo> m_nonHiddenFunctions;        
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
        
    ///////////////////////////////////////////////////////////////////////////
    
    AutoReg::AutoReg(   TestFunction function, 
                        const char* name,
                        const char* description,
                        const SourceLineInfo& lineInfo ) {
        registerTestCase( new FreeFunctionTestCase( function ), name, description, lineInfo );
    }    
    
    AutoReg::~AutoReg() {}
    
    void AutoReg::registerTestCase( ITestCase* testCase, 
                                    const char* name, 
                                    const char* description,
                                    const SourceLineInfo& lineInfo ) {
        getMutableRegistryHub().registerTest( TestCaseInfo( testCase, name, description, lineInfo ) );
    }
    
} // end namespace Catch

