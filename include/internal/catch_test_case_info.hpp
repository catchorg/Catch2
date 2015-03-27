/*
 *  Created by Phil on 14/08/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEST_CASE_INFO_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TEST_CASE_INFO_HPP_INCLUDED

#include "catch_test_spec.hpp"
#include "catch_test_case_info.h"
#include "catch_interfaces_testcase.h"
#include "catch_common.h"

namespace Catch {

    inline TestCaseInfo::SpecialProperties parseSpecialTag( std::string const& tag ) {
        if( startsWith( tag, "." ) ||
            tag == "hide" ||
            tag == "!hide" )
            return TestCaseInfo::IsHidden;
        else if( tag == "!throws" )
            return TestCaseInfo::Throws;
        else if( tag == "!shouldfail" )
            return TestCaseInfo::ShouldFail;
        else if( tag == "!mayfail" )
            return TestCaseInfo::MayFail;
        else
            return TestCaseInfo::None;
    }
    inline bool isReservedTag( std::string const& tag ) {
        return parseSpecialTag( tag ) == TestCaseInfo::None && tag.size() > 0 && !isalnum( tag[0] );
    }
    inline void enforceNotReservedTag( std::string const& tag, SourceLineInfo const& _lineInfo ) {
        if( isReservedTag( tag ) ) {
            {
                Colour colourGuard( Colour::Red );
                Catch::cerr()
                    << "Tag name [" << tag << "] not allowed.\n"
                    << "Tag names starting with non alpha-numeric characters are reserved\n";
            }
            {
                Colour colourGuard( Colour::FileName );
                Catch::cerr() << _lineInfo << std::endl;
            }
            exit(1);
        }
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

    TestCaseInfo::TestCaseInfo( std::string const& _name,
                                std::string const& _className,
                                std::string const& _description,
                                std::set<std::string> const& _tags,
                                SourceLineInfo const& _lineInfo )
    :   name( _name ),
        className( _className ),
        description( _description ),
        tags( _tags ),
        lineInfo( _lineInfo ),
        properties( None )
    {
        std::ostringstream oss;
        for( std::set<std::string>::const_iterator it = _tags.begin(), itEnd = _tags.end(); it != itEnd; ++it ) {
            oss << "[" << *it << "]";
            std::string lcaseTag = toLower( *it );
            properties = static_cast<SpecialProperties>( properties | parseSpecialTag( lcaseTag ) );
            lcaseTags.insert( lcaseTag );
        }
        tagsAsString = oss.str();
    }

    TestCaseInfo::TestCaseInfo( TestCaseInfo const& other )
    :   name( other.name ),
        className( other.className ),
        description( other.description ),
        tags( other.tags ),
        lcaseTags( other.lcaseTags ),
        tagsAsString( other.tagsAsString ),
        lineInfo( other.lineInfo ),
        properties( other.properties )
    {}

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

    TestCase::TestCase( TestCase const& other )
    :   TestCaseInfo( other ),
        test( other.test )
    {}

    TestCase TestCase::withName( std::string const& _newName ) const {
        TestCase other( *this );
        other.name = _newName;
        return other;
    }

    void TestCase::swap( TestCase& other ) {
        test.swap( other.test );
        name.swap( other.name );
        className.swap( other.className );
        description.swap( other.description );
        tags.swap( other.tags );
        lcaseTags.swap( other.lcaseTags );
        tagsAsString.swap( other.tagsAsString );
        std::swap( TestCaseInfo::properties, static_cast<TestCaseInfo&>( other ).properties );
        std::swap( lineInfo, other.lineInfo );
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
    TestCase& TestCase::operator = ( TestCase const& other ) {
        TestCase temp( other );
        swap( temp );
        return *this;
    }

    TestCaseInfo const& TestCase::getTestCaseInfo() const
    {
        return *this;
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TEST_CASE_INFO_HPP_INCLUDED
