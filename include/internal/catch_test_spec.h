/*
 *  Created by Phil on 14/8/2012.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEST_SPEC_H_INCLUDED
#define TWOBLUECUBES_CATCH_TEST_SPEC_H_INCLUDED

#include "catch_test_case_info.h"
#include "catch_tags.hpp"
#include "catch_common.h"

#include <string>
#include <vector>

namespace Catch {

    struct IfFilterMatches{ enum DoWhat {
        AutoDetectBehaviour,
        IncludeTests,
        ExcludeTests
    }; };

    class TestCaseFilter {
        enum WildcardPosition {
            NoWildcard = 0,
            WildcardAtStart = 1,
            WildcardAtEnd = 2,
            WildcardAtBothEnds = WildcardAtStart | WildcardAtEnd
        };

    public:
        TestCaseFilter( std::string const& testSpec, IfFilterMatches::DoWhat matchBehaviour = IfFilterMatches::AutoDetectBehaviour )
        :   m_stringToMatch( toLower( testSpec ) ),
            m_filterType( matchBehaviour ),
            m_wildcardPosition( NoWildcard )
        {
            if( m_filterType == IfFilterMatches::AutoDetectBehaviour ) {
                if( startsWith( m_stringToMatch, "exclude:" ) ) {
                    m_stringToMatch = m_stringToMatch.substr( 8 );
                    m_filterType = IfFilterMatches::ExcludeTests;
                }
                else if( startsWith( m_stringToMatch, "~" ) ) {
                    m_stringToMatch = m_stringToMatch.substr( 1 );
                    m_filterType = IfFilterMatches::ExcludeTests;
                }
                else {
                    m_filterType = IfFilterMatches::IncludeTests;
                }
            }

            if( startsWith( m_stringToMatch, "*" ) ) {
                m_stringToMatch = m_stringToMatch.substr( 1 );
                m_wildcardPosition = (WildcardPosition)( m_wildcardPosition | WildcardAtStart );
            }
            if( endsWith( m_stringToMatch, "*" ) ) {
                m_stringToMatch = m_stringToMatch.substr( 0, m_stringToMatch.size()-1 );
                m_wildcardPosition = (WildcardPosition)( m_wildcardPosition | WildcardAtEnd );
            }
        }

        IfFilterMatches::DoWhat getFilterType() const {
            return m_filterType;
        }

        bool shouldInclude( TestCase const& testCase ) const {
            return isMatch( testCase ) == (m_filterType == IfFilterMatches::IncludeTests);
        }
    private:

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#endif

        bool isMatch( TestCase const& testCase ) const {
            std::string name = testCase.getTestCaseInfo().name;
            toLowerInPlace( name );

            switch( m_wildcardPosition ) {
                case NoWildcard:
                    return m_stringToMatch == name;
                case WildcardAtStart:
                    return endsWith( name, m_stringToMatch );
                case WildcardAtEnd:
                    return startsWith( name, m_stringToMatch );
                case WildcardAtBothEnds:
                    return contains( name, m_stringToMatch );
            }
            throw std::logic_error( "Unhandled wildcard type" );
        }

#ifdef __clang__
#pragma clang diagnostic pop
#endif

        std::string m_stringToMatch;
        IfFilterMatches::DoWhat m_filterType;
        WildcardPosition m_wildcardPosition;
    };

    class TestCaseFilters {
    public:
        TestCaseFilters( std::string const& name ) : m_name( name ) {}

        std::string getName() const {
            return m_name;
        }

        void addFilter( TestCaseFilter const& filter ) {
            if( filter.getFilterType() == IfFilterMatches::ExcludeTests )
                m_exclusionFilters.push_back( filter );
            else
                m_inclusionFilters.push_back( filter );
        }

        void addTags( std::string const& tagPattern ) {
            TagExpression exp;
            TagExpressionParser( exp ).parse( tagPattern );

            m_tagExpressions.push_back( exp );
        }

        bool shouldInclude( TestCase const& testCase ) const {
            if( !m_tagExpressions.empty() ) {
                std::vector<TagExpression>::const_iterator it = m_tagExpressions.begin();
                std::vector<TagExpression>::const_iterator itEnd = m_tagExpressions.end();
                for(; it != itEnd; ++it )
                    if( it->matches( testCase.getTags() ) )
                        break;
                if( it == itEnd )
                    return false;
            }

            if( !m_inclusionFilters.empty() ) {
                std::vector<TestCaseFilter>::const_iterator it = m_inclusionFilters.begin();
                std::vector<TestCaseFilter>::const_iterator itEnd = m_inclusionFilters.end();
                for(; it != itEnd; ++it )
                    if( it->shouldInclude( testCase ) )
                        break;
                if( it == itEnd )
                    return false;
            }
            else if( m_exclusionFilters.empty() && m_tagExpressions.empty() ) {
                return !testCase.isHidden();
            }

            std::vector<TestCaseFilter>::const_iterator it = m_exclusionFilters.begin();
            std::vector<TestCaseFilter>::const_iterator itEnd = m_exclusionFilters.end();
            for(; it != itEnd; ++it )
                if( !it->shouldInclude( testCase ) )
                    return false;
            return true;
        }
    private:
        std::vector<TagExpression> m_tagExpressions;
        std::vector<TestCaseFilter> m_inclusionFilters;
        std::vector<TestCaseFilter> m_exclusionFilters;
        std::string m_name;
    };

}

#endif // TWOBLUECUBES_CATCH_TEST_SPEC_H_INCLUDED
