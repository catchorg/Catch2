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
        }
        
        ///////////////////////////////////////////////////////////////////////////
        virtual const std::vector<TestCaseInfo>& getAllTests
        ()
        const
        {
            return m_functionsInOrder;
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
        const char* description
    )
    {
        registerTestCase( new FreeFunctionTestCase( function ), name, description );
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
        const char* description 
    )
    {
        Hub::getTestCaseRegistry().registerTest( TestCaseInfo( testCase, name, description ) );
    }
    
} // end namespace Catch

