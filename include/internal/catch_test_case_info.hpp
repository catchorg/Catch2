/*
 *  Created by Phil on 14/08/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEST_CASE_INFO_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TEST_CASE_INFO_HPP_INCLUDED

#include "catch_tags.hpp"
#include "catch_test_case_info.h"
#include "catch_interfaces_testcase.h"

namespace Catch {

    
    TestCaseInfo::TestCaseInfo( ITestCase* testCase,
                                const std::string& className,
                                const std::string& name,
                                const std::string& description,
                                const SourceLineInfo& lineInfo )
    :   m_test( testCase ),
        m_className( className ),
        m_name( name ),
        m_description( description ),
        m_lineInfo( lineInfo ),
        m_isHidden( startsWith( name, "./" ) )
    {
        TagExtracter( m_tags ).parse( m_description );
        if( hasTag( "hide" ) )
            m_isHidden = true;
    }

    TestCaseInfo::TestCaseInfo()
    :   m_test( NULL ),
        m_className(),
        m_name(),
        m_description(),
        m_isHidden( false )
    {}

    TestCaseInfo::TestCaseInfo( const TestCaseInfo& other, const std::string& name )
    :   m_test( other.m_test ),
        m_className( other.m_className ),
        m_name( name ),
        m_description( other.m_description ),
        m_tags( other.m_tags ),
        m_lineInfo( other.m_lineInfo ),
        m_isHidden( other.m_isHidden )
    {}

    TestCaseInfo::TestCaseInfo( const TestCaseInfo& other )
    :   m_test( other.m_test ),
        m_className( other.m_className ),
        m_name( other.m_name ),
        m_description( other.m_description ),
        m_tags( other.m_tags ),
        m_lineInfo( other.m_lineInfo ),
        m_isHidden( other.m_isHidden )
    {}

    void TestCaseInfo::invoke() const {
        m_test->invoke();
    }

    const std::string& TestCaseInfo::getClassName() const {
        return m_className;
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
    bool TestCaseInfo::matchesTags( const std::string& tagPattern ) const {
        TagExpression exp;
        TagExpressionParser( exp ).parse( tagPattern );
        return exp.matches( m_tags );
    }
    const std::set<std::string>& TestCaseInfo::getTags() const {
        return m_tags;
    }

    void TestCaseInfo::swap( TestCaseInfo& other ) {
        m_test.swap( other.m_test );
        m_className.swap( other.m_className );
        m_name.swap( other.m_name );
        m_description.swap( other.m_description );
        std::swap( m_lineInfo, other.m_lineInfo );
    }

    bool TestCaseInfo::operator == ( const TestCaseInfo& other ) const {
        return  m_test.get() == other.m_test.get() &&
                m_name == other.m_name &&
                m_className == other.m_className;
    }

    bool TestCaseInfo::operator < ( const TestCaseInfo& other ) const {
        return m_name < other.m_name;
    }
    TestCaseInfo& TestCaseInfo::operator = ( const TestCaseInfo& other ) {
        TestCaseInfo temp( other );
        swap( temp );
        return *this;
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TEST_CASE_INFO_HPP_INCLUDED
