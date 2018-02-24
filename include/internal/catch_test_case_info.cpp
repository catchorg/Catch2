/*
 *  Created by Phil on 14/08/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_test_case_info.h"
#include "catch_enforce.h"
#include "catch_test_registry.h"
#include "catch_test_spec.h"
#include "catch_interfaces_testcase.h"
#include "catch_string_manip.h"

#include <algorithm>
#include <cctype>
#include <exception>
#include <sstream>
#include <type_traits>

namespace Catch {

namespace {

    size_t sizeOfExtraTags(StringRef filename) {
        // Iterate the filename backward to get the parts between last / and last .
        size_t idx = filename.size();
        while (idx > 0 && filename[idx-1] != '.') { --idx; }

        size_t lastDot = idx - 1;
        while (idx > 0 && filename[idx-1] != '/' && filename[idx-1] != '\\') { --idx; }
        size_t lastSlash = idx - 1;

        // +3 for '[', ']' and '#'
        return lastDot - lastSlash - 1 + 3;
    }

    StringRef extractFilenamePart(StringRef filename) {
        size_t idx = filename.size();
        while (idx > 0 && filename[idx - 1] != '.') { --idx; }

        size_t lastDot = idx - 1;
        while (idx > 0 && filename[idx - 1] != '/' && filename[idx - 1] != '\\') { --idx; }

        return StringRef(&filename[idx], lastDot - idx);
    }

    TestCaseProperties parseSpecialTag(StringRef const& tag) {
        if ((tag.size() >= 2 && tag[1] == '.') ||
                   tag == "[!hide]"_sr) {
            return TestCaseProperties::IsHidden;
        } else if (tag == "[!throws]"_sr) {
            return TestCaseProperties::Throws;
        } else if (tag == "[!shouldfail]"_sr) {
            return TestCaseProperties::ShouldFail;
        } else if (tag == "[!mayfail]"_sr) {
            return TestCaseProperties::MayFail;
        } else if (tag == "[!nonportable]"_sr) {
            return TestCaseProperties::NonPortable;
        } else if (tag == "[!benchmark]"_sr) {
            return TestCaseProperties::Benchmark | TestCaseProperties::IsHidden;
        } else {
            return TestCaseProperties::None;
        }
    }

    bool isReservedTag(StringRef const& tag) {
        return parseSpecialTag(tag) == TestCaseProperties::None && tag.size() >= 2 && !std::isalnum(tag[1]);
    }

    void enforceNotReservedTag(StringRef const& tag, SourceLineInfo const& _lineInfo) {
        CATCH_ENFORCE(!isReservedTag(tag),
                      "Tag name: [" << tag << "] is not allowed.\n"
                      << "Tag names starting with non alpha-numeric characters are reserved\n"
                      << _lineInfo);
    }

    bool applies(TestCaseProperties props) {
        return !!(static_cast<std::underlying_type<TestCaseProperties>::type>(props));
    }

    std::string getDefaultName() {
        static size_t counter = 0;
        return "Anonymous test case " + std::to_string(++counter);
    }

} // Anonymous namespace



    TestCase makeTestCase(  ITestInvoker* _testCase,
                            std::string const& _className,
                            NameAndTags const& nameAndDescription,
                            SourceLineInfo const& _lineInfo )
    {
        auto info = std::make_shared<TestCaseInfo>(_className, nameAndDescription, _lineInfo);
        return TestCase( _testCase, std::move(info) );
    }

    bool operator<(Tag lhs, Tag rhs) {
        return lhs.original.substr(1, lhs.original.size() - 2) < rhs.original.substr(1, rhs.original.size() - 2);
    }

    bool operator==(Tag lhs, Tag rhs) {
        return lhs.original == rhs.original;
    }

    TestCaseProperties operator|(TestCaseProperties lhs, TestCaseProperties rhs) {
        return static_cast<TestCaseProperties>(
            static_cast<std::underlying_type<TestCaseProperties>::type>(lhs) |
            static_cast<std::underlying_type<TestCaseProperties>::type>(rhs)
        );
    }

    TestCaseProperties& operator|=(TestCaseProperties& lhs, TestCaseProperties rhs) {
        lhs = static_cast<TestCaseProperties>(
            static_cast<std::underlying_type<TestCaseProperties>::type>(lhs) |
            static_cast<std::underlying_type<TestCaseProperties>::type>(rhs)
        );
        return lhs;
    }

    TestCaseProperties operator&(TestCaseProperties lhs, TestCaseProperties rhs) {
        return static_cast<TestCaseProperties>(
            static_cast<std::underlying_type<TestCaseProperties>::type>(lhs) &
            static_cast<std::underlying_type<TestCaseProperties>::type>(rhs)
        );
    }


    TestCaseInfo::TestCaseInfo( std::string const& _className,
                                NameAndTags const& nameAndDescription,
                                SourceLineInfo const& _lineInfo )
    :   name( nameAndDescription.name.empty()? getDefaultName() : std::string(nameAndDescription.name) ),
        className( _className ),
        lineInfo( _lineInfo )
    {

        // Reserve enough space for underlying tags and fill it with both.
        auto backingSize = nameAndDescription.tags.size() + sizeOfExtraTags(_lineInfo.file);
        tagsBacking.reserve(backingSize);
        lcaseTagsBacking.reserve(backingSize);

        tagsBacking += nameAndDescription.tags;
        lcaseTagsBacking += tagsBacking;
        toLowerInPlace(lcaseTagsBacking);

        // Now we can parse the description and extract tags
        bool inTag = false;
        size_t tagStart = 0;

        for (size_t idx = 0; idx < tagsBacking.size(); ++idx) {
            char c = tagsBacking[idx];
            if (!inTag) {
                if (c == '[') {
                    inTag = true;
                    tagStart = idx;
                } else {
                    description += c;
                }
            } else {
                if (c == ']') {
                    inTag = false;
                    StringRef tagRef(&tagsBacking[tagStart], idx - tagStart + 1);
                    enforceNotReservedTag(tagRef, _lineInfo);
                    properties |= parseSpecialTag(tagRef);

                    tags.emplace_back(tagRef, StringRef(&lcaseTagsBacking[tagStart], idx - tagStart + 1));
               }
            }
        }

        // If the test case is hidden, we have to also add [.] to both tag ref vectors
        // We can use string literal so they do not have to be added to the backing store
        if ( isHidden() ) {
            tags.emplace_back("[.]"_sr, "[.]"_sr);
        }

        // Sort and unique tag refs
        std::sort(tags.begin(), tags.end());
        tags.erase(std::unique(tags.begin(), tags.end()), tags.end());
    }

    bool TestCaseInfo::isHidden() const {
        return applies( properties & TestCaseProperties::IsHidden );
    }
    bool TestCaseInfo::throws() const {
        return applies( properties & TestCaseProperties::Throws );
    }
    bool TestCaseInfo::okToFail() const {
        return applies( properties & (TestCaseProperties::ShouldFail | TestCaseProperties::MayFail ) );
    }
    bool TestCaseInfo::expectedToFail() const {
        return applies( properties & (TestCaseProperties::ShouldFail ) );
    }

    std::string TestCaseInfo::tagsAsString() const {
        std::string ret;
        std::size_t full_size = 0;
        for (const auto& tag : tags) {
            full_size += tag.original.size();
        }
        ret.reserve(full_size);
        for (const auto& tag : tags) {
            ret += tag.original;
        }

        return ret;
    }

    void TestCaseInfo::applyFilenameTag() {
        auto startIdx = tagsBacking.size();
        tagsBacking += "[#";
        tagsBacking += extractFilenamePart(lineInfo.file);
        tagsBacking += ']';
        auto endIdx = tagsBacking.size();
        lcaseTagsBacking += "[#";
        lcaseTagsBacking += extractFilenamePart(lineInfo.file);
        lcaseTagsBacking += ']';
        tags.emplace_back(StringRef(&tagsBacking[startIdx], endIdx - startIdx), StringRef(&lcaseTagsBacking[startIdx], endIdx - startIdx));
    }


    TestCase::TestCase( ITestInvoker* testCase, std::shared_ptr<TestCaseInfo> info ) : test( testCase ), info(std::move(info)) {}

    void TestCase::invoke() const {
        test->invoke();
    }

    bool TestCase::operator == ( TestCase const& other ) const {
        return  test.get() == other.test.get() &&
                info->name == other.info->name &&
                info->className == other.info->className;
    }

    bool TestCase::operator < ( TestCase const& other ) const {
        return info->name < other.info->name;
    }

    std::shared_ptr<TestCaseInfo> const& TestCase::getTestCaseInfo() const {
        return info;
    }

} // end namespace Catch
