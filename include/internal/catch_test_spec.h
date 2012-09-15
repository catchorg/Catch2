/*
 *  Created by Phil on 14/8/2012.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TESTSPEC_H_INCLUDED
#define TWOBLUECUBES_CATCH_TESTSPEC_H_INCLUDED

#include "catch_test_case_info.h"

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
        TestCaseFilter( const std::string& testSpec, IfFilterMatches::DoWhat matchBehaviour = IfFilterMatches::AutoDetectBehaviour )
        :   m_stringToMatch( testSpec ),
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

            if( m_stringToMatch[0] == '*' ) {
                m_stringToMatch = m_stringToMatch.substr( 1 );
                m_wildcardPosition = (WildcardPosition)( m_wildcardPosition | WildcardAtStart );
            }
            if( m_stringToMatch[m_stringToMatch.size()-1] == '*' ) {
                m_stringToMatch = m_stringToMatch.substr( 0, m_stringToMatch.size()-1 );
                m_wildcardPosition = (WildcardPosition)( m_wildcardPosition | WildcardAtEnd );
            }
        }

        IfFilterMatches::DoWhat getFilterType() const {
            return m_filterType;
        }
        
        bool shouldInclude( const TestCaseInfo& testCase ) const {
            return isMatch( testCase ) == (m_filterType == IfFilterMatches::IncludeTests);
        }
    private:

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#endif

        bool isMatch( const TestCaseInfo& testCase ) const {
            const std::string& name = testCase.getName();

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
        TestCaseFilters( const std::string& name ) : m_name( name ) {}

        std::string getName() const {
            return m_name;
        }
        
        void addFilter( const TestCaseFilter& filter ) {
            if( filter.getFilterType() == IfFilterMatches::ExcludeTests )
                m_exclusionFilters.push_back( filter );
            else
                m_inclusionFilters.push_back( filter );
        }

        bool shouldInclude( const TestCaseInfo& testCase ) const {
            if( !m_inclusionFilters.empty() ) {
                std::vector<TestCaseFilter>::const_iterator it = m_inclusionFilters.begin();
                std::vector<TestCaseFilter>::const_iterator itEnd = m_inclusionFilters.end();
                for(; it != itEnd; ++it )
                    if( it->shouldInclude( testCase ) )
                        break;
                if( it == itEnd )
                    return false;
            }
            else if( m_exclusionFilters.empty() ) {
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
        std::vector<TestCaseFilter> m_inclusionFilters;
        std::vector<TestCaseFilter> m_exclusionFilters;
        std::string m_name;
    };

}

#endif // TWOBLUECUBES_CATCH_TESTSPEC_H_INCLUDED
