/*
 *  Created by Phil on 13/7/2015.
 *  Copyright 2015 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_WILDCARD_PATTERN_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_WILDCARD_PATTERN_HPP_INCLUDED

#include "catch_common.h"


namespace Catch
{
    class WildcardPattern {
        enum WildcardPosition {
            NoWildcard = 0,
            WildcardAtStart = 1,
            WildcardAtEnd = 2,
            WildcardAtBothEnds = WildcardAtStart | WildcardAtEnd
        };

    public:

        WildcardPattern( std::string const& pattern, CaseSensitive::Choice caseSensitivity );
        virtual ~WildcardPattern() = default;
        virtual bool matches( std::string const& str ) const;

    private:
        std::string normaliseString( std::string const& str ) const;
        CaseSensitive::Choice m_caseSensitivity;
        WildcardPosition m_wildcard = NoWildcard;
        std::string m_pattern;
    };
}

#endif // TWOBLUECUBES_CATCH_WILDCARD_PATTERN_HPP_INCLUDED
