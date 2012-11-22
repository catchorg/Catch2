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
    
    class TestCase {
    public:
        TestCase();

        TestCase(   ITestCase* testCase,
                        const std::string& className,
                        const std::string& name,
                        const std::string& description,
                        const SourceLineInfo& lineInfo );


        TestCase( const TestCase& other, const std::string& name );
        TestCase( const TestCase& other );

        void invoke() const;

        const std::string& getClassName() const;
        const std::string& getName() const;
        const std::string& getDescription() const;
        const SourceLineInfo& getLineInfo() const;
        bool isHidden() const;
        bool hasTag( const std::string& tag ) const;
        bool matchesTags( const std::string& tagPattern ) const;
        const std::set<std::string>& getTags() const;
        
        void swap( TestCase& other );
        bool operator == ( const TestCase& other ) const;
        bool operator < ( const TestCase& other ) const;
        TestCase& operator = ( const TestCase& other );

    private:
        Ptr<ITestCase> m_test;
        std::string m_className;
        std::string m_name;
        std::string m_description;
        std::set<std::string> m_tags;
        SourceLineInfo m_lineInfo;
        bool m_isHidden;
    };
}

#endif // TWOBLUECUBES_CATCH_TEST_CASE_INFO_H_INCLUDED
