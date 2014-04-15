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
#include "catch_ptr.hpp"

#include <string>
#include <set>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

namespace Catch {

    struct ITestCase;

    struct TestCaseInfo {
        TestCaseInfo(   std::string const& _name,
                        std::string const& _className,
                        std::string const& _description,
                        std::set<std::string> const& _tags,
                        bool _isHidden,
                        SourceLineInfo const& _lineInfo );

        TestCaseInfo( TestCaseInfo const& other );

        std::string name;
        std::string className;
        std::string description;
        std::set<std::string> tags;
        std::string tagsAsString;
        SourceLineInfo lineInfo;
        bool isHidden;
        bool throws;
    };

    class TestCase : protected TestCaseInfo {
    public:

        TestCase( ITestCase* testCase, TestCaseInfo const& info );
        TestCase( TestCase const& other );

        TestCase withName( std::string const& _newName ) const;

        void invoke() const;

        TestCaseInfo const& getTestCaseInfo() const;

        bool isHidden() const;
        bool throws() const;
        bool hasTag( std::string const& tag ) const;
        bool matchesTags( std::string const& tagPattern ) const;
        std::set<std::string> const& getTags() const;

        void swap( TestCase& other );
        bool operator == ( TestCase const& other ) const;
        bool operator < ( TestCase const& other ) const;
        TestCase& operator = ( TestCase const& other );

    private:
        Ptr<ITestCase> test;
    };

    TestCase makeTestCase(  ITestCase* testCase,
                            std::string const& className,
                            std::string const& name,
                            std::string const& description,
                            SourceLineInfo const& lineInfo );
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_CATCH_TEST_CASE_INFO_H_INCLUDED
