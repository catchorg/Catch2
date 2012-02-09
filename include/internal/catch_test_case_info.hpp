/*
 *  catch_test_case_info.hpp
 *  Catch
 *
 *  Created by Phil on 29/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef TWOBLUECUBES_CATCH_TESTCASEINFO_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TESTCASEINFO_HPP_INCLUDED

#include "catch_interfaces_testcase.h"
#include <map>
#include <string>

namespace Catch
{
    class TestCaseInfo
    {
    public:
        ///////////////////////////////////////////////////////////////////////
        TestCaseInfo
        (
            ITestCase* testCase, 
            const char* name, 
            const char* description,
            const char* filename,
            std::size_t line
        )
        :   m_test( testCase ),
            m_name( name ),
            m_description( description ),
            m_filename( filename ),
            m_line( line )
        {
        }

        ///////////////////////////////////////////////////////////////////////
        TestCaseInfo
        ()
        :   m_test( NULL ),
            m_name(),
            m_description(),
            m_filename(),
            m_line( 0 )
        {
        }
        
        ///////////////////////////////////////////////////////////////////////
        TestCaseInfo
        (
            const TestCaseInfo& other
        )
        :   m_test( other.m_test->clone() ),
            m_name( other.m_name ),
            m_description( other.m_description ),
            m_filename( other.m_filename ),
            m_line( other.m_line )
        {
        }
        
        ///////////////////////////////////////////////////////////////////////
        TestCaseInfo
        (
            const TestCaseInfo& other,
            const std::string& name
        )
        :   m_test( other.m_test->clone() ),
            m_name( name ),
            m_description( other.m_description ),
            m_filename( other.m_filename ),
            m_line( other.m_line )
        {
        }
        
        ///////////////////////////////////////////////////////////////////////
        TestCaseInfo& operator = 
        (
            const TestCaseInfo& other 
        )
        {
            TestCaseInfo temp( other );
            swap( temp );
            return *this;
        }
        
        ///////////////////////////////////////////////////////////////////////
        ~TestCaseInfo
        ()
        {
            delete m_test;
        }
        
        ///////////////////////////////////////////////////////////////////////
        void invoke
        ()
        const
        {
            m_test->invoke();
        }
        
        ///////////////////////////////////////////////////////////////////////
        const std::string& getName
        ()
        const
        {
            return m_name;
        }

        ///////////////////////////////////////////////////////////////////////
        const std::string& getDescription
        ()
        const
        {
            return m_description;
        }

        ///////////////////////////////////////////////////////////////////////
        const std::string& getFilename
        ()
        const
        {
            return m_filename;
        }

        ///////////////////////////////////////////////////////////////////////
        std::size_t getLine
        ()
        const
        {
            return m_line;
        }
        
        ///////////////////////////////////////////////////////////////////////
        bool isHidden
        ()
        const
        {
            return m_name.size() >= 2 && m_name[0] == '.' && m_name[1] == '/';
        }        
        
        ///////////////////////////////////////////////////////////////////////
        void swap
        (
            TestCaseInfo& other
        )
        {
            std::swap( m_test, other.m_test );
            m_name.swap( other.m_name );
            m_description.swap( other.m_description );
        }
        
        ///////////////////////////////////////////////////////////////////////
        bool operator == 
        (
            const TestCaseInfo& other
        )
        const
        {
            return *m_test == *other.m_test && m_name == other.m_name;
        }
        
        ///////////////////////////////////////////////////////////////////////
        bool operator <
        (
            const TestCaseInfo& other
        ) 
        const
        {
            return m_name < other.m_name;
        }

    private:
        ITestCase* m_test;
        std::string m_name;
        std::string m_description;
        std::string m_filename;
        std::size_t m_line;
        
    };
    
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////    
    
    class TestSpec
    {
    public:
        ///////////////////////////////////////////////////////////////////////        
        TestSpec
        (
            const std::string& rawSpec
        )
        :   m_rawSpec( rawSpec ),
            m_isWildcarded( false )
        {
            if( m_rawSpec[m_rawSpec.size()-1] == '*' )
            {
                m_rawSpec = m_rawSpec.substr( 0, m_rawSpec.size()-1 );
                m_isWildcarded = true;
            }
        }
        
        ///////////////////////////////////////////////////////////////////////        
        bool matches
        (
            const std::string& testName
        )
        const
        {
            if( !m_isWildcarded )
                return m_rawSpec == testName;
            else
                return testName.size() >= m_rawSpec.size() && testName.substr( 0, m_rawSpec.size() ) == m_rawSpec;            
        }
        
    private:
        std::string m_rawSpec;
        bool m_isWildcarded;
    };
}

#endif // TWOBLUECUBES_CATCH_TESTCASEINFO_HPP_INCLUDED
