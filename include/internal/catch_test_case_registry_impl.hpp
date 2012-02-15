/*
 *  catch_test_case_registry_impl.hpp
 *  Catch
 *
 *  Created by Phil on 7/1/2011
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "catch_test_registry.hpp"
#include "catch_test_case_info.hpp"
#include "catch_hub.h"

#include <vector>
#include <set>
#include <sstream>

#include <iostream> // !TBD DBG
namespace Catch
{
    class TestRegistry : public ITestCaseRegistry
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        TestRegistry
        ()
        : m_unnamedCount( 0 )
        {
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void registerTest
        (
            const TestCaseInfo& testInfo
        )
        {
            if( testInfo.getName() == "" )
            {
                std::ostringstream oss;
                oss << testInfo.getName() << "unnamed/" << ++m_unnamedCount;
                return registerTest( TestCaseInfo( testInfo, oss.str() ) );
            }
            if( m_functions.find( testInfo ) == m_functions.end() )
            {
                m_functions.insert( testInfo );
                m_functionsInOrder.push_back( testInfo );
            }
            else
            {
                const TestCaseInfo& prev = *m_functions.find( testInfo );
                std::cerr   << "error: TEST_CASE( \"" << testInfo.getName() << "\" ) already defined.\n"
                            << "\tFirst seen at " << SourceLineInfo( prev.getFilename(), prev.getLine() ) << "\n"
                            << "\tRedefined at " << SourceLineInfo( testInfo.getFilename(), testInfo.getLine() ) << std::endl;
                exit(1);
            }
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual const std::vector<TestCaseInfo>& getAllTests
        ()
        const
        {
            return m_functionsInOrder;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual std::vector<TestCaseInfo> getMatchingTestCases
        ( 
            const std::string& rawTestSpec 
        )
        {
            TestSpec testSpec( rawTestSpec );
            
            std::vector<TestCaseInfo> testList;
            std::vector<TestCaseInfo>::const_iterator it = m_functionsInOrder.begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = m_functionsInOrder.end();
            for(; it != itEnd; ++it )
            {
                if( testSpec.matches( it->getName() ) )
                {
                    testList.push_back( *it );
                }
            }
            return testList;
        }
        
    private:
        
        std::set<TestCaseInfo> m_functions;
        std::vector<TestCaseInfo> m_functionsInOrder;
        size_t m_unnamedCount;
    };

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    
    
    struct FreeFunctionTestCase : ITestCase
    {
        ///////////////////////////////////////////////////////////////////////////
        FreeFunctionTestCase
        (
            TestFunction fun
        )
        : m_fun( fun )
        {}
        
        ///////////////////////////////////////////////////////////////////////////
        virtual void invoke
        ()
        const
        {
            m_fun();
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual ITestCase* clone
        ()
        const
        {
            return new FreeFunctionTestCase( m_fun );
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual bool operator == 
        (
            const ITestCase& other 
        )
        const
        {
            const FreeFunctionTestCase* ffOther = dynamic_cast<const FreeFunctionTestCase*> ( &other );
            return ffOther && m_fun == ffOther->m_fun;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual bool operator <
        (
            const ITestCase& other 
        )
        const
        {
            const FreeFunctionTestCase* ffOther = dynamic_cast<const FreeFunctionTestCase*> ( &other );
            return ffOther && m_fun < ffOther->m_fun;
        }
        
    private:
        TestFunction m_fun;
    };
        
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////////////
    AutoReg::AutoReg
    (
        TestFunction function, 
        const char* name,
        const char* description,
        const char* filename,
        std::size_t line
    )
    {
        registerTestCase( new FreeFunctionTestCase( function ), name, description, filename, line );
    }    
    
    ///////////////////////////////////////////////////////////////////////////
    AutoReg::~AutoReg
    ()
    {
    }
    
    ///////////////////////////////////////////////////////////////////////////
    void AutoReg::registerTestCase
    (
        ITestCase* testCase, 
        const char* name, 
        const char* description,
        const char* filename,
        std::size_t line
    )
    {
        Hub::getTestCaseRegistry().registerTest( TestCaseInfo( testCase, name, description, filename, line ) );
    }
    
} // end namespace Catch

