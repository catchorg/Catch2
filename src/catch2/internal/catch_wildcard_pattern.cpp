
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_wildcard_pattern.hpp>
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_string_manip.hpp>

namespace Catch {

    namespace {
        bool glob_match(const char* pattern, const char* str) {
            while (*pattern) {
                if (*pattern == '\\') {
                    pattern++;
                    if (!*pattern) {
                        return *str == '\\' && glob_match(pattern, str + 1);
                    }
                    if (*pattern != *str) {
                        return false;
                    }
                    pattern++;
                    str++;
                } else if (*pattern == '*') {
                    while (*pattern == '*') {
                        pattern++;
                    }
                    if (!*pattern) {
                        return true;
                    }
                    while (*str) {
                        if (glob_match(pattern, str)) {
                            return true;
                        }
                        str++;
                    }
                    return false;
                } else {
                    if (*pattern != *str) {
                        return false;
                    }
                    pattern++;
                    str++;
                }
            }
            return *str == '\0';
        }
    }

    WildcardPattern::WildcardPattern( std::string const& pattern,
                                      CaseSensitive caseSensitivity )
    :   m_caseSensitivity( caseSensitivity ),
        m_pattern( normaliseString( pattern ) )
    {}

    bool WildcardPattern::matches( std::string const& str ) const {
        return glob_match( m_pattern.c_str(), normaliseString( str ).c_str() );
    }

    std::string WildcardPattern::normaliseString( std::string const& str ) const {
        return trim( m_caseSensitivity == CaseSensitive::No ? toLower( str ) : str );
    }
}
