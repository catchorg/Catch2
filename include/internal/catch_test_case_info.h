/*
 *  Created by Phil on 29/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TESTCASEINFO_H_INCLUDED
#define TWOBLUECUBES_CATCH_TESTCASEINFO_H_INCLUDED

#include "catch_common.h"

#include <string>

namespace Catch {

    struct ITestCase;

    class TestCaseInfo {
    public:
        TestCaseInfo();

        TestCaseInfo(   ITestCase* testCase,
                        const char* name, 
                        const char* description,
                        const SourceLineInfo& lineInfo );


        TestCaseInfo( const TestCaseInfo& other, const std::string& name );

        void invoke() const;
        const std::string& getName() const;
        const std::string& getDescription() const;
        const SourceLineInfo& getLineInfo() const;
        bool isHidden() const;
        
        void swap( TestCaseInfo& other );
        bool operator == ( const TestCaseInfo& other ) const;
        bool operator < ( const TestCaseInfo& other ) const;

    private:
        Ptr<ITestCase> m_test;
        std::string m_name;
        std::string m_description;
        SourceLineInfo m_lineInfo;        
    };
    
    ///////////////////////////////////////////////////////////////////////////    
    
    class TestSpec {
    public:
        TestSpec( const std::string& rawSpec )
        :   m_rawSpec( rawSpec ),
            m_isWildcarded( false ) {
            
            if( m_rawSpec[m_rawSpec.size()-1] == '*' ) {
                m_rawSpec = m_rawSpec.substr( 0, m_rawSpec.size()-1 );
                m_isWildcarded = true;
            }
        }
        
        bool matches ( const std::string& testName ) const {
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

#endif // TWOBLUECUBES_CATCH_TESTCASEINFO_H_INCLUDED
