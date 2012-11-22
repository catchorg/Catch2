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

    
    TestCase::TestCase( ITestCase* testCase,
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

    TestCase::TestCase()
    :   m_test( NULL ),
        m_className(),
        m_name(),
        m_description(),
        m_isHidden( false )
    {}

    TestCase::TestCase( const TestCase& other, const std::string& name )
    :   m_test( other.m_test ),
        m_className( other.m_className ),
        m_name( name ),
        m_description( other.m_description ),
        m_tags( other.m_tags ),
        m_lineInfo( other.m_lineInfo ),
        m_isHidden( other.m_isHidden )
    {}

    TestCase::TestCase( const TestCase& other )
    :   m_test( other.m_test ),
        m_className( other.m_className ),
        m_name( other.m_name ),
        m_description( other.m_description ),
        m_tags( other.m_tags ),
        m_lineInfo( other.m_lineInfo ),
        m_isHidden( other.m_isHidden )
    {}

    void TestCase::invoke() const {
        m_test->invoke();
    }

    const std::string& TestCase::getClassName() const {
        return m_className;
    }
    const std::string& TestCase::getName() const {
        return m_name;
    }
    const std::string& TestCase::getDescription() const {
        return m_description;
    }
    const SourceLineInfo& TestCase::getLineInfo() const {
        return m_lineInfo;
    }

    bool TestCase::isHidden() const {
        return m_isHidden;
    }

    bool TestCase::hasTag( const std::string& tag ) const {
        return m_tags.find( tag ) != m_tags.end();
    }
    bool TestCase::matchesTags( const std::string& tagPattern ) const {
        TagExpression exp;
        TagExpressionParser( exp ).parse( tagPattern );
        return exp.matches( m_tags );
    }
    const std::set<std::string>& TestCase::getTags() const {
        return m_tags;
    }

    void TestCase::swap( TestCase& other ) {
        m_test.swap( other.m_test );
        m_className.swap( other.m_className );
        m_name.swap( other.m_name );
        m_description.swap( other.m_description );
        std::swap( m_lineInfo, other.m_lineInfo );
    }

    bool TestCase::operator == ( const TestCase& other ) const {
        return  m_test.get() == other.m_test.get() &&
                m_name == other.m_name &&
                m_className == other.m_className;
    }

    bool TestCase::operator < ( const TestCase& other ) const {
        return m_name < other.m_name;
    }
    TestCase& TestCase::operator = ( const TestCase& other ) {
        TestCase temp( other );
        swap( temp );
        return *this;
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TEST_CASE_INFO_HPP_INCLUDED
