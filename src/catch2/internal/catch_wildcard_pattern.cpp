
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_wildcard_pattern.hpp>
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_string_manip.hpp>
#include <catch2/internal/catch_stringref.hpp>
#include <catch2/internal/catch_case_insensitive_comparisons.hpp>

namespace Catch {

    WildcardPattern::WildcardPattern( std::string const& pattern,
                                      CaseSensitive caseSensitivity )
    :   m_caseSensitivity( caseSensitivity ),
        m_pattern( trim(pattern) )
    {
        if( startsWith( m_pattern, '*' ) ) {
            m_pattern = m_pattern.erase( 0, 1 ); // effectively pop_front
            m_wildcard = static_cast<WildcardPosition>( m_wildcard | WildcardAtStart );
        }
        if( endsWith( m_pattern, '*' ) ) {
            m_pattern.pop_back();
            m_wildcard = static_cast<WildcardPosition>( m_wildcard | WildcardAtEnd );
        }
    }

    bool WildcardPattern::matches( StringRef input ) const {
        input = trim( input );
        if ( input.size() < m_pattern.size() ) { return false; }

        auto casedMatch = [&]( StringRef lhs, StringRef rhs ) {
            if ( m_caseSensitivity == CaseSensitive::Yes ) {
                return lhs == rhs;
            } else {
                Detail::CaseInsensitiveEqualTo eq;
                return eq( lhs, rhs );
            }
        };

        switch ( m_wildcard ) {
        case NoWildcard:
            return casedMatch(input, m_pattern);
        case WildcardAtStart:
            return casedMatch(
                input.substr( input.size() - m_pattern.size(), input.size() ),
                m_pattern );
        case WildcardAtEnd:
            return casedMatch( input.substr( 0, m_pattern.size() ), m_pattern );
        case WildcardAtBothEnds: {
            // TBD: This has terrible performance, but it is also the least
            //      common use case. Once the StringContains matcher gets
            //      efficient implementation, we should reuse it here.
            const size_t lastOffset = input.size() - m_pattern.size();
            for ( size_t offset = 0; offset <= lastOffset; ++offset ) {
                if ( casedMatch( input.substr(offset, m_pattern.size()), m_pattern ) ) { return true; }
            }
            return false;
        }
        default:
            CATCH_INTERNAL_ERROR( "Unknown enum" );
        }
    }
}
