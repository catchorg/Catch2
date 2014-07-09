/*
 *  Created by Phil on 14/8/2012.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEST_SPEC_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TEST_SPEC_HPP_INCLUDED

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

#include "catch_test_case_info.h"

#include <string>
#include <vector>

namespace Catch {

    class TestSpec {
        struct Pattern : SharedImpl<> {
            virtual ~Pattern();
            virtual bool matches( TestCaseInfo const& testCase ) const = 0;
        };
        class NamePattern : public Pattern {
            enum WildcardPosition {
                NoWildcard = 0,
                WildcardAtStart = 1,
                WildcardAtEnd = 2,
                WildcardAtBothEnds = WildcardAtStart | WildcardAtEnd
            };

        public:
            NamePattern( std::string const& name ) : m_name( toLower( name ) ), m_wildcard( NoWildcard ) {
                if( startsWith( m_name, "*" ) ) {
                    m_name = m_name.substr( 1 );
                    m_wildcard = WildcardAtStart;
                }
                if( endsWith( m_name, "*" ) ) {
                    m_name = m_name.substr( 0, m_name.size()-1 );
                    m_wildcard = static_cast<WildcardPosition>( m_wildcard | WildcardAtEnd );
                }
            }
            virtual ~NamePattern();
            virtual bool matches( TestCaseInfo const& testCase ) const {
                switch( m_wildcard ) {
                    case NoWildcard:
                        return m_name == toLower( testCase.name );
                    case WildcardAtStart:
                        return endsWith( toLower( testCase.name ), m_name );
                    case WildcardAtEnd:
                        return startsWith( toLower( testCase.name ), m_name );
                    case WildcardAtBothEnds:
                        return contains( toLower( testCase.name ), m_name );
                }

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#endif
                throw std::logic_error( "Unknown enum" );
#ifdef __clang__
#pragma clang diagnostic pop
#endif
            }
        private:
            std::string m_name;
            WildcardPosition m_wildcard;
        };
        class TagPattern : public Pattern {
        public:
            TagPattern( std::string const& tag ) : m_tag( toLower( tag ) ) {}
            virtual ~TagPattern();
            virtual bool matches( TestCaseInfo const& testCase ) const {
                return testCase.lcaseTags.find( m_tag ) != testCase.lcaseTags.end();
            }
        private:
            std::string m_tag;
        };
        class ExcludedPattern : public Pattern {
        public:
            ExcludedPattern( Ptr<Pattern> const& underlyingPattern ) : m_underlyingPattern( underlyingPattern ) {}
            virtual ~ExcludedPattern();
            virtual bool matches( TestCaseInfo const& testCase ) const { return !m_underlyingPattern->matches( testCase ); }
        private:
            Ptr<Pattern> m_underlyingPattern;
        };

        struct Filter {
            std::vector<Ptr<Pattern> > m_patterns;

            bool matches( TestCaseInfo const& testCase ) const {
                // All patterns in a filter must match for the filter to be a match
                for( std::vector<Ptr<Pattern> >::const_iterator it = m_patterns.begin(), itEnd = m_patterns.end(); it != itEnd; ++it )
                    if( !(*it)->matches( testCase ) )
                        return false;
                    return true;
            }
        };

    public:
        bool hasFilters() const {
            return !m_filters.empty();
        }
        bool matches( TestCaseInfo const& testCase ) const {
            // A TestSpec matches if any filter matches
            for( std::vector<Filter>::const_iterator it = m_filters.begin(), itEnd = m_filters.end(); it != itEnd; ++it )
                if( it->matches( testCase ) )
                    return true;
            return false;
        }

    private:
        std::vector<Filter> m_filters;

        friend class TestSpecParser;
    };
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_CATCH_TEST_SPEC_HPP_INCLUDED
