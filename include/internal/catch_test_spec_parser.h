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

#include "catch_test_spec.h"
#include "catch_string_manip.h"
#include "catch_interfaces_tag_alias_registry.h"

namespace Catch {

    class TestSpecParser {
        enum Mode{ None, Name, QuotedName, Tag, EscapedName };
        Mode m_mode = None;
        bool m_exclusion = false;
        std::size_t m_pos = 0;
        std::string m_arg;
        std::string m_substring;
        std::string m_patternName;
        std::vector<std::size_t> m_escapeChars;
        TestSpec::Filter m_currentFilter;
        TestSpec m_testSpec;
        ITagAliasRegistry const* m_tagAliases = nullptr;

    public:
        TestSpecParser( ITagAliasRegistry const& tagAliases );

        TestSpecParser& parse( std::string const& arg );
        TestSpec testSpec();

    private:
        void visitChar( char c );
        void startNewMode( Mode mode );
        bool processNoneChar( char c );
        void processNameChar( char c );
        bool processOtherChar( char c );
        void endMode();
        void escape();
        bool isControlChar( char c ) const;

        template<typename T>
        void addPattern() {
            std::string token = m_patternName;
            for( std::size_t i = 0; i < m_escapeChars.size(); ++i )
                token = token.substr( 0, m_escapeChars[i] - i ) + token.substr( m_escapeChars[i] -i +1 );
            m_escapeChars.clear();
            if( startsWith( token, "exclude:" ) ) {
                m_exclusion = true;
                token = token.substr( 8 );
            }
            if( !token.empty() ) {
                TestSpec::PatternPtr pattern = std::make_shared<T>( token, m_substring );
                if( m_exclusion )
                    pattern = std::make_shared<TestSpec::ExcludedPattern>( pattern );
                m_currentFilter.m_patterns.push_back( pattern );
            }
            m_substring.clear();
            m_patternName.clear();
            m_exclusion = false;
            m_mode = None;
        }

        void addFilter();
    };
    TestSpec parseTestSpec( std::string const& arg );

} // namespace Catch

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_CATCH_TEST_SPEC_PARSER_HPP_INCLUDED
