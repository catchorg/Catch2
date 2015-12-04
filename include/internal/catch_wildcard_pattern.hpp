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

        WildcardPattern( std::string const& pattern, CaseSensitive::Choice caseSensitivity )
        :   m_caseSensitivity( caseSensitivity ),
            m_wildcard( NoWildcard ),
            m_pattern( adjustCase( pattern ) )
        {
            if( startsWith( m_pattern, "*" ) ) {
                m_pattern = m_pattern.substr( 1 );
                m_wildcard = WildcardAtStart;
            }
            if( endsWith( m_pattern, "*" ) ) {
                m_pattern = m_pattern.substr( 0, m_pattern.size()-1 );
                m_wildcard = static_cast<WildcardPosition>( m_wildcard | WildcardAtEnd );
            }
        }
        virtual ~WildcardPattern();
        virtual bool matches( std::string const& str ) const {
            switch( m_wildcard ) {
                case NoWildcard:
                    return m_pattern == adjustCase( str );
                case WildcardAtStart:
                    return endsWith( adjustCase( str ), m_pattern );
                case WildcardAtEnd:
                    return startsWith( adjustCase( str ), m_pattern );
                case WildcardAtBothEnds:
                    return contains( adjustCase( str ), m_pattern );
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
        std::string adjustCase( std::string const& str ) const {
            return m_caseSensitivity == CaseSensitive::No ? toLower( str ) : str;
        }
        CaseSensitive::Choice m_caseSensitivity;
        WildcardPosition m_wildcard;
        std::string m_pattern;
    };
}

#endif // TWOBLUECUBES_CATCH_WILDCARD_PATTERN_HPP_INCLUDED
