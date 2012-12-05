/*
 *  Created by Phil on 29/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEST_CASE_INFO_H_INCLUDED
#define TWOBLUECUBES_CATCH_TEST_CASE_INFO_H_INCLUDED

#include "catch_common.h"

#include <string>
#include <set>

namespace Catch {

    struct ITestCase;

    struct TestCaseInfo {
        TestCaseInfo(   const std::string& _name,
                        const std::string& _className,
                        const std::string& _description,
                        const std::set<std::string>& _tags,
                        bool _isHidden,
                        const SourceLineInfo& _lineInfo );

        TestCaseInfo( const TestCaseInfo& other );

        std::string name;
        std::string className;
        std::string description;
        std::set<std::string> tags;
        SourceLineInfo lineInfo;
        bool isHidden;
    };
    
    class TestCase : protected TestCaseInfo {
    public:

        TestCase( ITestCase* testCase, const TestCaseInfo& info );
        TestCase( const TestCase& other );

        TestCase withName( const std::string& _newName ) const;

        void invoke() const;

        const TestCaseInfo& getTestCaseInfo() const;

        bool isHidden() const;
        bool hasTag( const std::string& tag ) const;
        bool matchesTags( const std::string& tagPattern ) const;
        const std::set<std::string>& getTags() const;
        
        void swap( TestCase& other );
        bool operator == ( const TestCase& other ) const;
        bool operator < ( const TestCase& other ) const;
        TestCase& operator = ( const TestCase& other );

    private:
        Ptr<ITestCase> test;
    };

    TestCase makeTestCase(  ITestCase* testCase,
                            const std::string& className,
                            const std::string& name,
                            const std::string& description,
                            const SourceLineInfo& lineInfo );
}

#endif // TWOBLUECUBES_CATCH_TEST_CASE_INFO_H_INCLUDED
