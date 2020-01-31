/*
 *  Created by Phil on 14/08/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_test_case_info.h"
#include "catch_enforce.h"
#include "catch_test_spec.h"
#include "catch_interfaces_testcase.h"
#include "catch_string_manip.h"

#include <cctype>
#include <exception>
#include <algorithm>
#include <sstream>

namespace Catch {

    namespace {
        TestCaseInfo::SpecialProperties parseSpecialTag( std::string const& tag ) {
            if( startsWith( tag, '.' ) ||
                tag == "!hide" )
                return TestCaseInfo::IsHidden;
            else if( tag == "!throws" )
                return TestCaseInfo::Throws;
            else if( tag == "!shouldfail" )
                return TestCaseInfo::ShouldFail;
            else if( tag == "!mayfail" )
                return TestCaseInfo::MayFail;
            else if( tag == "!nonportable" )
                return TestCaseInfo::NonPortable;
            else if( tag == "!benchmark" )
                return static_cast<TestCaseInfo::SpecialProperties>( TestCaseInfo::Benchmark | TestCaseInfo::IsHidden );
            else
                return TestCaseInfo::None;
        }
        bool isReservedTag( std::string const& tag ) {
            return parseSpecialTag( tag ) == TestCaseInfo::None && tag.size() > 0 && !std::isalnum( static_cast<unsigned char>(tag[0]) );
        }
        void enforceNotReservedTag( std::string const& tag, SourceLineInfo const& _lineInfo ) {
            CATCH_ENFORCE( !isReservedTag(tag),
                          "Tag name: [" << tag << "] is not allowed.\n"
                          << "Tag names starting with non alphanumeric characters are reserved\n"
                          << _lineInfo );
        }
    }

    TestCase makeTestCase(  ITestInvoker* _testCase,
                            std::string const& _className,
                            NameAndTags const& nameAndTags,
                            SourceLineInfo const& _lineInfo )
    {
        bool isHidden = false;

        // Parse out tags
        std::vector<std::string> tags;
        std::string desc, tag;
        bool inTag = false;
        for (char c : nameAndTags.tags) {
            if( !inTag ) {
                if( c == '[' )
                    inTag = true;
                else
                    desc += c;
            }
            else {
                if( c == ']' ) {
                    TestCaseInfo::SpecialProperties prop = parseSpecialTag( tag );
                    if( ( prop & TestCaseInfo::IsHidden ) != 0 )
                        isHidden = true;
                    else if( prop == TestCaseInfo::None )
                        enforceNotReservedTag( tag, _lineInfo );

                    // Merged hide tags like `[.approvals]` should be added as
                    // `[.][approvals]`. The `[.]` is added at later point, so
                    // we only strip the prefix
                    if (startsWith(tag, '.') && tag.size() > 1) {
                        tag.erase(0, 1);
                    }
                    tags.push_back( tag );
                    tag.clear();
                    inTag = false;
                }
                else
                    tag += c;
            }
        }
        if( isHidden ) {
            // Add all "hidden" tags to make them behave identically
            tags.insert( tags.end(), { ".", "!hide" } );
        }

        TestCaseInfo info( static_cast<std::string>(nameAndTags.name), _className, desc, tags, _lineInfo );
        return TestCase( _testCase, std::move(info) );
    }

    void setTags( TestCaseInfo& testCaseInfo, std::vector<std::string> tags ) {
        std::sort(begin(tags), end(tags));
        tags.erase(std::unique(begin(tags), end(tags)), end(tags));
        testCaseInfo.lcaseTags.clear();

        for( auto const& tag : tags ) {
            std::string lcaseTag = toLower( tag );
            testCaseInfo.properties = static_cast<TestCaseInfo::SpecialProperties>( testCaseInfo.properties | parseSpecialTag( lcaseTag ) );
            testCaseInfo.lcaseTags.push_back( lcaseTag );
        }
        testCaseInfo.tags = std::move(tags);
    }

    TestCaseInfo::TestCaseInfo( std::string const& _name,
                                std::string const& _className,
                                std::string const& _description,
                                std::vector<std::string> const& _tags,
                                SourceLineInfo const& _lineInfo )
    :   name( _name ),
        className( _className ),
        description( _description ),
        lineInfo( _lineInfo ),
        properties( None )
    {
        setTags( *this, _tags );
    }

    bool TestCaseInfo::isHidden() const {
        return ( properties & IsHidden ) != 0;
    }
    bool TestCaseInfo::throws() const {
        return ( properties & Throws ) != 0;
    }
    bool TestCaseInfo::okToFail() const {
        return ( properties & (ShouldFail | MayFail ) ) != 0;
    }
    bool TestCaseInfo::expectedToFail() const {
        return ( properties & (ShouldFail ) ) != 0;
    }

    std::string TestCaseInfo::tagsAsString() const {
        std::string ret;
        // '[' and ']' per tag
        std::size_t full_size = 2 * tags.size();
        for (const auto& tag : tags) {
            full_size += tag.size();
        }
        ret.reserve(full_size);
        for (const auto& tag : tags) {
            ret.push_back('[');
            ret.append(tag);
            ret.push_back(']');
        }

        return ret;
    }


    TestCase::TestCase( ITestInvoker* testCase, TestCaseInfo&& info ) : TestCaseInfo( std::move(info) ), test( testCase ) {}


    TestCase TestCase::withName( std::string const& _newName ) const {
        TestCase other( *this );
        other.name = _newName;
        return other;
    }

    void TestCase::invoke() const {
        test->invoke();
    }

    bool TestCase::operator == ( TestCase const& other ) const {
        return  test.get() == other.test.get() &&
                name == other.name &&
                className == other.className;
    }

    bool TestCase::operator < ( TestCase const& other ) const {
        return name < other.name;
    }

    TestCaseInfo const& TestCase::getTestCaseInfo() const
    {
        return *this;
    }

} // end namespace Catch
