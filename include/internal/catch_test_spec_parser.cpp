/*
 *  Created by Martin on 19/07/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_test_spec_parser.h"

namespace Catch {

    TestSpecParser::TestSpecParser( ITagAliasRegistry const& tagAliases ) : m_tagAliases( &tagAliases ) {}

    TestSpecParser& TestSpecParser::parse( std::string const& arg ) {
        m_mode = None;
        m_exclusion = false;
        m_start = std::string::npos;
        m_arg = m_tagAliases->expandAliases( arg );
        m_escapeChars.clear();
        for( m_pos = 0; m_pos < m_arg.size(); ++m_pos )
            visitChar( m_arg[m_pos] );
        if( m_mode == Name )
            addPattern<TestSpec::NamePattern>();
        return *this;
    }
    TestSpec TestSpecParser::testSpec() {
        addFilter();
        return m_testSpec;
    }

    void TestSpecParser::visitChar( char c ) {
        if( m_mode == None ) {
            switch( c ) {
            case ' ': return;
            case '~': m_exclusion = true; return;
            case '[': return startNewMode( Tag, ++m_pos );
            case '"': return startNewMode( QuotedName, ++m_pos );
            case '\\': return escape();
            default: startNewMode( Name, m_pos ); break;
            }
        }
        if( m_mode == Name ) {
            if( c == ',' ) {
                addPattern<TestSpec::NamePattern>();
                addFilter();
            }
            else if( c == '[' ) {
                if( subString() == "exclude:" )
                    m_exclusion = true;
                else
                    addPattern<TestSpec::NamePattern>();
                startNewMode( Tag, ++m_pos );
            }
            else if( c == '\\' )
                escape();
        }
        else if( m_mode == EscapedName )
            m_mode = Name;
        else if( m_mode == QuotedName && c == '"' )
            addPattern<TestSpec::NamePattern>();
        else if( m_mode == Tag && c == ']' )
            addPattern<TestSpec::TagPattern>();
    }
    void TestSpecParser::startNewMode( Mode mode, std::size_t start ) {
        m_mode = mode;
        m_start = start;
    }
    void TestSpecParser::escape() {
        if( m_mode == None )
            m_start = m_pos;
        m_mode = EscapedName;
        m_escapeChars.push_back( m_pos );
    }
    std::string TestSpecParser::subString() const { return m_arg.substr( m_start, m_pos - m_start ); }

    void TestSpecParser::addFilter() {
        if( !m_currentFilter.m_patterns.empty() ) {
            m_testSpec.m_filters.push_back( m_currentFilter );
            m_currentFilter = TestSpec::Filter();
        }
    }

    TestSpec parseTestSpec( std::string const& arg ) {
        return TestSpecParser( ITagAliasRegistry::get() ).parse( arg ).testSpec();
    }

} // namespace Catch
