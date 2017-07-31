/*
 *  Created by Phil on 15/5/2013.
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEST_SPEC_PARSER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TEST_SPEC_PARSER_HPP_INCLUDED

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

#include "catch_test_spec.hpp"
#include "catch_interfaces_tag_alias_registry.h"

namespace Catch {

    class TestSpecParser {
        enum Mode{ None, Name, QuotedName, Tag, EscapedName };
        Mode m_mode;
        bool m_exclusion;
        std::size_t m_start, m_pos;
        std::string m_arg;
        std::vector<std::size_t> m_escapeChars;
        TestSpec::Filter m_currentFilter;
        TestSpec m_testSpec;
        ITagAliasRegistry const* m_tagAliases;

    public:
        TestSpecParser( ITagAliasRegistry const& tagAliases ) :m_mode(None), m_exclusion(false), m_start(0), m_pos(0), m_tagAliases( &tagAliases ) {}

        TestSpecParser& parse( std::string const& arg ) {
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
        TestSpec testSpec() {
            addFilter();
            return m_testSpec;
        }
    private:
        void visitChar( char c ) {
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
        void startNewMode( Mode mode, std::size_t start ) {
            m_mode = mode;
            m_start = start;
        }
        void escape() {
            if( m_mode == None )
                m_start = m_pos;
            m_mode = EscapedName;
            m_escapeChars.push_back( m_pos );
        }
        std::string subString() const { return m_arg.substr( m_start, m_pos - m_start ); }
        template<typename T>
        void addPattern() {
            std::string token = subString();
            for( size_t i = 0; i < m_escapeChars.size(); ++i )
                token = token.substr( 0, m_escapeChars[i]-m_start-i ) + token.substr( m_escapeChars[i]-m_start-i+1 );
            m_escapeChars.clear();
            if( startsWith( token, "exclude:" ) ) {
                m_exclusion = true;
                token = token.substr( 8 );
            }
            if( !token.empty() ) {
                Ptr<TestSpec::Pattern> pattern = new T( token );
                if( m_exclusion )
                    pattern = new TestSpec::ExcludedPattern( pattern );
                m_currentFilter.m_patterns.push_back( pattern );
            }
            m_exclusion = false;
            m_mode = None;
        }
        void addFilter() {
            if( !m_currentFilter.m_patterns.empty() ) {
                m_testSpec.m_filters.push_back( m_currentFilter );
                m_currentFilter = TestSpec::Filter();
            }
        }
    };
    inline TestSpec parseTestSpec( std::string const& arg ) {
        return TestSpecParser( ITagAliasRegistry::get() ).parse( arg ).testSpec();
    }

} // namespace Catch

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_CATCH_TEST_SPEC_PARSER_HPP_INCLUDED
