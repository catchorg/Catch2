/*
 *  Created by Martin on 19/07/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_wildcard_pattern.h"
#include "catch_enforce.h"
#include "catch_string_manip.h"

#include <sstream>

namespace Catch {

    WildcardPattern::WildcardPattern( std::string const& pattern,
                                      CaseSensitive::Choice caseSensitivity )
    :   m_caseSensitivity( caseSensitivity ),
        m_pattern( adjustCase( pattern ) )
    {
        if( startsWith( m_pattern, '*' ) ) {
            m_pattern = m_pattern.substr( 1 );
            m_wildcard = WildcardAtStart;
        }
        if( endsWith( m_pattern, '*' ) ) {
            m_pattern = m_pattern.substr( 0, m_pattern.size()-1 );
            m_wildcard = static_cast<WildcardPosition>( m_wildcard | WildcardAtEnd );
        }
    }

    bool WildcardPattern::matches( std::string const& str ) const {
        switch( m_wildcard ) {
            case NoWildcard:
                return m_pattern == adjustCase( str );
            case WildcardAtStart:
                return endsWith( adjustCase( str ), m_pattern );
            case WildcardAtEnd:
                return startsWith( adjustCase( str ), m_pattern );
            case WildcardAtBothEnds:
                return contains( adjustCase( str ), m_pattern );
            default:
                CATCH_INTERNAL_ERROR( "Unknown enum" );
        }
    }

    std::string WildcardPattern::adjustCase( std::string const& str ) const {
        return m_caseSensitivity == CaseSensitive::No ? toLower( str ) : str;
    }
}
