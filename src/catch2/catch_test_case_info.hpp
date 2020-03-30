/*
 *  Created by Phil on 29/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEST_CASE_INFO_H_INCLUDED
#define TWOBLUECUBES_CATCH_TEST_CASE_INFO_H_INCLUDED

#include <catch2/internal/catch_common.hpp>
#include <catch2/catch_stringref.hpp>
#include <catch2/internal/catch_test_registry.hpp>

#include <string>
#include <vector>
#include <memory>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

namespace Catch {

    struct Tag {
        Tag(StringRef original_, StringRef lowerCased_):
            original(original_), lowerCased(lowerCased_)
        {}
        StringRef original, lowerCased;
    };

    struct ITestInvoker;

    enum class TestCaseProperties : uint8_t {
        None = 0,
        IsHidden = 1 << 1,
        ShouldFail = 1 << 2,
        MayFail = 1 << 3,
        Throws = 1 << 4,
        NonPortable = 1 << 5,
        Benchmark = 1 << 6
    };


    struct TestCaseInfo : NonCopyable {

        TestCaseInfo(std::string const& _className,
                     NameAndTags const& _tags,
                     SourceLineInfo const& _lineInfo);

        bool isHidden() const;
        bool throws() const;
        bool okToFail() const;
        bool expectedToFail() const;

        // Adds the tag(s) with test's filename (for the -# flag)
        void addFilenameTag();


        std::string tagsAsString() const;

        std::string name;
        std::string className;
    private:
        std::string backingTags, backingLCaseTags;
        // Internally we copy tags to the backing storage and then add
        // refs to this storage to the tags vector.
        void internalAppendTag(StringRef tagString);
    public:
        std::vector<Tag> tags;
        SourceLineInfo lineInfo;
        TestCaseProperties properties = TestCaseProperties::None;
    };

    class TestCaseHandle {
        TestCaseInfo* m_info;
        ITestInvoker* m_invoker;
    public:
        TestCaseHandle(TestCaseInfo* info, ITestInvoker* invoker) :
            m_info(info), m_invoker(invoker) {}

        void invoke() const {
            m_invoker->invoke();
        }

        TestCaseInfo const& getTestCaseInfo() const;

        bool operator== ( TestCaseHandle const& rhs ) const;
        bool operator < ( TestCaseHandle const& rhs ) const;
    };

    std::unique_ptr<TestCaseInfo> makeTestCaseInfo(  std::string const& className,
                            NameAndTags const& nameAndTags,
                            SourceLineInfo const& lineInfo );
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_CATCH_TEST_CASE_INFO_H_INCLUDED
