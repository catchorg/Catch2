/*
 *  Created by Phil on 14/8/2012.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TESTSPEC_H_INCLUDED
#define TWOBLUECUBES_CATCH_TESTSPEC_H_INCLUDED

#include <string>

namespace Catch {
    
    struct IfFilterMatches{ enum DoWhat {
        IncludeTests,
        ExcludeTests
    }; };

    class TestCaseFilter {
    public:
        TestCaseFilter( const std::string& testSpec, IfFilterMatches::DoWhat matchBehaviour = IfFilterMatches::IncludeTests )
        :   m_testSpec( testSpec ),
            m_filterType( matchBehaviour ),
            m_isWildcarded( false )
        {
            if( m_testSpec[m_testSpec.size()-1] == '*' ) {
                m_testSpec = m_testSpec.substr( 0, m_testSpec.size()-1 );
                m_isWildcarded = true;
            }
        }

        IfFilterMatches::DoWhat getFilterType() const {
            return m_filterType;
        }
        
        bool shouldInclude( const TestCaseInfo& testCase ) const {
            return isMatch( testCase ) == (m_filterType == IfFilterMatches::IncludeTests);
        }
    private:

        bool isMatch( const TestCaseInfo& testCase ) const {
            const std::string& name = testCase.getName();
            if( !m_isWildcarded )
                return m_testSpec == name;
            else
                return name.size() >= m_testSpec.size() && name.substr( 0, m_testSpec.size() ) == m_testSpec;
        }

        std::string m_testSpec;
        IfFilterMatches::DoWhat m_filterType;
        bool m_isWildcarded;
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
