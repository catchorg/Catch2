/*
 *  Created by Martin on 19/07/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_test_spec.h"
#include "catch_string_manip.h"
#include "catch_interfaces_config.h"

#include <algorithm>
#include <string>
#include <vector>
#include <memory>

namespace Catch {

    TestSpec::Pattern::Pattern( std::string const& name )
    : m_name( name )
    {}

    TestSpec::Pattern::~Pattern() = default;

    std::string const& TestSpec::Pattern::name() const {
        return m_name;
    }


    TestSpec::NamePattern::NamePattern( std::string const& name, std::string const& filterString )
    : Pattern( filterString )
    , m_wildcardPattern( toLower( name ), CaseSensitive::No )
    {}

    bool TestSpec::NamePattern::matches( TestCaseInfo const& testCase ) const {
        return m_wildcardPattern.matches( testCase.name );
    }


    TestSpec::TagPattern::TagPattern( std::string const& tag, std::string const& filterString )
    : Pattern( filterString )
    , m_tag( toLower( tag ) )
    {}

    bool TestSpec::TagPattern::matches( TestCaseInfo const& testCase ) const {
        return std::find(begin(testCase.lcaseTags),
                         end(testCase.lcaseTags),
                         m_tag) != end(testCase.lcaseTags);
    }


    TestSpec::ExcludedPattern::ExcludedPattern( PatternPtr const& underlyingPattern )
    : Pattern( underlyingPattern->name() )
    , m_underlyingPattern( underlyingPattern )
    {}

    bool TestSpec::ExcludedPattern::matches( TestCaseInfo const& testCase ) const {
        return !m_underlyingPattern->matches( testCase );
    }


    bool TestSpec::Filter::matches( TestCaseInfo const& testCase ) const {
        return std::all_of( m_patterns.begin(), m_patterns.end(), [&]( PatternPtr const& p ){ return p->matches( testCase ); } );
    }

    std::string TestSpec::Filter::name() const {
        std::string name;
        for( auto const& p : m_patterns )
            name += p->name();
        return name;
    }


    bool TestSpec::hasFilters() const {
        return !m_filters.empty();
    }

    bool TestSpec::matches( TestCaseInfo const& testCase ) const {
        return std::any_of( m_filters.begin(), m_filters.end(), [&]( Filter const& f ){ return f.matches( testCase ); } );
    }

    TestSpec::Matches TestSpec::matchesByFilter( std::vector<TestCase> const& testCases, IConfig const& config ) const
    {
        Matches matches( m_filters.size() );
        std::transform( m_filters.begin(), m_filters.end(), matches.begin(), [&]( Filter const& filter ){
            std::vector<TestCase const*> currentMatches;
            for( auto const& test : testCases )
                if( isThrowSafe( test, config ) && filter.matches( test ) )
                    currentMatches.emplace_back( &test );
            return FilterMatch{ filter.name(), currentMatches };
        } );
        return matches;
    }
    
    const TestSpec::vectorStrings& TestSpec::getInvalidArgs() const{
        return  (m_invalidArgs);
    }

}
