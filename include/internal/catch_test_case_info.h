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
#include "catch_stringref.h"

#include <string>
#include <vector>
#include <memory>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

namespace Catch {

    struct ITestInvoker;
    struct NameAndTags;

    struct Tag {
        Tag(StringRef original, StringRef lCased) : original(original), lowerCased(lCased) {}
        StringRef original, lowerCased;
    };

    bool operator <  (Tag lhs, Tag rhs);
    bool operator == (Tag lhs, Tag rhs);

    enum class TestCaseProperties {
        None = 0,
        IsHidden = 1 << 1,
        ShouldFail = 1 << 2,
        MayFail = 1 << 3,
        Throws = 1 << 4,
        NonPortable = 1 << 5,
        Benchmark = 1 << 6
    };

    TestCaseProperties  operator |  (TestCaseProperties lhs, TestCaseProperties rhs);
    TestCaseProperties& operator |= (TestCaseProperties& lhs, TestCaseProperties rhs);
    TestCaseProperties  operator &  (TestCaseProperties lhs, TestCaseProperties rhs);


    struct TestCaseInfo {

        TestCaseInfo(   std::string const& _className,
                        NameAndTags const& nameAndDescription,
                        SourceLineInfo const& _lineInfo );

        TestCaseInfo(TestCaseInfo const&) = delete;
        TestCaseInfo(TestCaseInfo&&) = delete;
        TestCaseInfo& operator=(TestCaseInfo const&) = delete;
        TestCaseInfo& operator=(TestCaseInfo&&) = delete;

        bool isHidden() const;
        bool throws() const;
        bool okToFail() const;
        bool expectedToFail() const;

        std::string tagsAsString() const;

        void applyFilenameTag();

        std::string name;
        std::string className;

        // This will be deprecated soon, but for now we keep it
        std::string description;

        // To save allocations, tags are non-owning substrings of 2 backing strings
        std::vector<Tag> tags;
    private:
        std::string tagsBacking;
        std::string lcaseTagsBacking;
    public:
        SourceLineInfo lineInfo;
        TestCaseProperties properties = TestCaseProperties::None;
    };

    class TestCase {
    public:

        TestCase( ITestInvoker* testCase, std::shared_ptr<TestCaseInfo> info );

        void invoke() const;

        std::shared_ptr<TestCaseInfo> const& getTestCaseInfo() const;

        bool operator == ( TestCase const& other ) const;
        bool operator < ( TestCase const& other ) const;

    private:
        std::shared_ptr<ITestInvoker> test;
        std::shared_ptr<TestCaseInfo> info;
    };

    TestCase makeTestCase(  ITestInvoker* testCase,
                            std::string const& className,
                            NameAndTags const& nameAndDescription,
                            SourceLineInfo const& lineInfo );
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_CATCH_TEST_CASE_INFO_H_INCLUDED
