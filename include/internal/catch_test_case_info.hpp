/*
 *  Created by Phil on 14/08/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TESTCASEINFO_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TESTCASEINFO_HPP_INCLUDED

#include "catch_test_case_info.h"
#include "catch_interfaces_testcase.h"

namespace Catch {

    namespace {
        void extractTags( std::string& str, std::set<std::string>& tags ) {
            std::string remainder;
            std::size_t last = 0;
            std::size_t begin = str.find_first_of( '[' );
            while( begin != std::string::npos ) {
                std::size_t end = str.find_first_of( ']', begin );
                if( end != std::string::npos ) {
                    std::string tag = str.substr( begin+1, end-begin-1 );
                    tags.insert( tag );
                    if( begin - last > 0 )
                        remainder += str.substr( last, begin-last );
                    last = end+1;
                }
                else if( begin != str.size()-1 ) {
                    end = begin+1;
                }
                else {
                    break;
                }
                begin = str.find_first_of( '[', end );
            }
            if( !tags.empty() ) {
                if( last < str.size() )
                    str = remainder + str.substr( last );
                else
                    str = remainder;
            }
        }
    }
    TestCaseInfo::TestCaseInfo( ITestCase* testCase,
                                const char* name,
                                const char* description,
                                const SourceLineInfo& lineInfo )
    :   m_test( testCase ),
        m_name( name ),
        m_description( description ),
        m_lineInfo( lineInfo ),
        m_isHidden( startsWith( name, "./" ) )
    {
        extractTags( m_description, m_tags );
        if( hasTag( "hide" ) )
            m_isHidden = true;
    }

    TestCaseInfo::TestCaseInfo()
    :   m_test( NULL ),
        m_name(),
        m_description(),
        m_isHidden( false )
    {}

    TestCaseInfo::TestCaseInfo( const TestCaseInfo& other, const std::string& name )
    :   m_test( other.m_test ),
        m_name( name ),
        m_description( other.m_description ),
        m_tags( other.m_tags ),
        m_lineInfo( other.m_lineInfo ),
        m_isHidden( other.m_isHidden )
    {}

    TestCaseInfo::TestCaseInfo( const TestCaseInfo& other )
    :   m_test( other.m_test ),
        m_name( other.m_name ),
        m_description( other.m_description ),
        m_tags( other.m_tags ),
        m_lineInfo( other.m_lineInfo ),
        m_isHidden( other.m_isHidden )
    {}

    void TestCaseInfo::invoke() const {
        m_test->invoke();
    }

    const std::string& TestCaseInfo::getName() const {
        return m_name;
    }

    const std::string& TestCaseInfo::getDescription() const {
        return m_description;
    }

    const SourceLineInfo& TestCaseInfo::getLineInfo() const {
        return m_lineInfo;
    }

    bool TestCaseInfo::isHidden() const {
        return m_isHidden;
    }

    bool TestCaseInfo::hasTag( const std::string& tag ) const {
        return m_tags.find( tag ) != m_tags.end();
    }
    const std::set<std::string>& TestCaseInfo::tags() const {
        return m_tags;
    }

    void TestCaseInfo::swap( TestCaseInfo& other ) {
        m_test.swap( other.m_test );
        m_name.swap( other.m_name );
        m_description.swap( other.m_description );
        m_lineInfo.swap( other.m_lineInfo );
    }

    bool TestCaseInfo::operator == ( const TestCaseInfo& other ) const {
        return m_test.get() == other.m_test.get() && m_name == other.m_name;
    }

    bool TestCaseInfo::operator < ( const TestCaseInfo& other ) const {
        return m_name < other.m_name;
    }
    TestCaseInfo& TestCaseInfo::operator = ( const TestCaseInfo& other ) {
        TestCaseInfo temp( other );
        swap( temp );
        return *this;
    }
}

#endif // TWOBLUECUBES_CATCH_TESTCASEINFO_HPP_INCLUDED
