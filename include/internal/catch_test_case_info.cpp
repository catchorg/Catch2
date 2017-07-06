/*
 *  Created by Phil on 14/08/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_test_spec.hpp"
#include "catch_test_case_info.h"
#include "catch_interfaces_testcase.h"
#include "catch_common.h"

#include <cctype>
#include <exception>

namespace Catch {

    inline TestCaseInfo::SpecialProperties parseSpecialTag( std::string const& tag ) {
        if( startsWith( tag, '.' ) ||
            tag == "hide" ||
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
        else
            return TestCaseInfo::None;
    }
    inline bool isReservedTag( std::string const& tag ) {
        return parseSpecialTag( tag ) == TestCaseInfo::None && tag.size() > 0 && !std::isalnum( tag[0] );
    }
    inline void enforceNotReservedTag( std::string const& tag, SourceLineInfo const& _lineInfo ) {
        CATCH_ENFORCE( !isReservedTag(tag),
                      "Tag name: [" << tag << "] is not allowed.\n"
                      << "Tag names starting with non alpha-numeric characters are reserved\n"
                      << _lineInfo );
    }

    TestCase makeTestCase(  ITestCase* _testCase,
                            std::string const& _className,
                            std::string const& _name,
                            std::string const& _descOrTags,
                            SourceLineInfo const& _lineInfo )
    {
        bool isHidden( startsWith( _name, "./" ) ); // Legacy support

        // Parse out tags
        std::set<std::string> tags;
        std::string desc, tag;
        bool inTag = false;
        for( std::size_t i = 0; i < _descOrTags.size(); ++i ) {
            char c = _descOrTags[i];
            if( !inTag ) {
                if( c == '[' )
                    inTag = true;
                else
                    desc += c;
            }
            else {
                if( c == ']' ) {
                    TestCaseInfo::SpecialProperties prop = parseSpecialTag( tag );
                    if( prop == TestCaseInfo::IsHidden )
                        isHidden = true;
                    else if( prop == TestCaseInfo::None )
                        enforceNotReservedTag( tag, _lineInfo );

                    tags.insert( tag );
                    tag.clear();
                    inTag = false;
                }
                else
                    tag += c;
            }
        }
        if( isHidden ) {
            tags.insert( "hide" );
            tags.insert( "." );
        }

        TestCaseInfo info( _name, _className, desc, tags, _lineInfo );
        return TestCase( _testCase, info );
    }

    void setTags( TestCaseInfo& testCaseInfo, std::set<std::string> const& tags )
    {
        testCaseInfo.tags = tags;
        testCaseInfo.lcaseTags.clear();

        std::ostringstream oss;
        for( auto const& tag : tags ) {
            oss << '[' << tag << ']';
            std::string lcaseTag = toLower( tag );
            testCaseInfo.properties = static_cast<TestCaseInfo::SpecialProperties>( testCaseInfo.properties | parseSpecialTag( lcaseTag ) );
            testCaseInfo.lcaseTags.insert( lcaseTag );
        }
        testCaseInfo.tagsAsString = oss.str();
    }

    TestCaseInfo::TestCaseInfo( std::string const& _name,
                                std::string const& _className,
                                std::string const& _description,
                                std::set<std::string> const& _tags,
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


    TestCase::TestCase( ITestCase* testCase, TestCaseInfo const& info ) : TestCaseInfo( info ), test( testCase ) {}


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
