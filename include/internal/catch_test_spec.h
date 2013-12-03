/*
 *  Created by Phil on 14/8/2012.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEST_SPEC_H_INCLUDED
#define TWOBLUECUBES_CATCH_TEST_SPEC_H_INCLUDED

#include "catch_tags.h"

#include <string>
#include <vector>

namespace Catch {

    class TestCase;

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
        TestCaseFilter( std::string const& testSpec, IfFilterMatches::DoWhat matchBehaviour = IfFilterMatches::AutoDetectBehaviour );

        IfFilterMatches::DoWhat getFilterType() const;
        bool shouldInclude( TestCase const& testCase ) const;

    private:
        bool isMatch( TestCase const& testCase ) const;

        std::string m_stringToMatch;
        IfFilterMatches::DoWhat m_filterType;
        WildcardPosition m_wildcardPosition;
    };

    class TestCaseFilters {
    public:
        TestCaseFilters( std::string const& name );
        std::string getName() const;
        void addFilter( TestCaseFilter const& filter );
        void addTags( std::string const& tagPattern );
        bool shouldInclude( TestCase const& testCase ) const;

    private:
        std::vector<TagExpression> m_tagExpressions;
        std::vector<TestCaseFilter> m_inclusionFilters;
        std::vector<TestCaseFilter> m_exclusionFilters;
        std::string m_name;
    };

}

#endif // TWOBLUECUBES_CATCH_TEST_SPEC_H_INCLUDED
