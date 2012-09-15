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
#include <set>

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
        TestCaseInfo( const TestCaseInfo& other );

        void invoke() const;
        const std::string& getName() const;
        const std::string& getDescription() const;
        const SourceLineInfo& getLineInfo() const;
        bool isHidden() const;
        bool hasTag( const std::string& tag ) const;
        const std::set<std::string>& tags() const;
        
        void swap( TestCaseInfo& other );
        bool operator == ( const TestCaseInfo& other ) const;
        bool operator < ( const TestCaseInfo& other ) const;
        TestCaseInfo& operator = ( const TestCaseInfo& other );

    private:
        Ptr<ITestCase> m_test;
        std::string m_name;
        std::string m_description;
        std::set<std::string> m_tags;
        SourceLineInfo m_lineInfo;
        bool m_isHidden;
    };
}

#endif // TWOBLUECUBES_CATCH_TESTCASEINFO_H_INCLUDED
