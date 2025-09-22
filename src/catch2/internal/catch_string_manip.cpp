
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_move_and_forward.hpp>
#include <catch2/internal/catch_string_manip.hpp>

#include <ostream>
#include <cstring>
#include <cctype>
#include <string_view>
#include <vector>

namespace Catch {

    bool startsWith( std::string_view s, std::string_view prefix ) {
        return s.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), s.begin());
    }
    bool startsWith( std::string_view s, char prefix ) {
        return !s.empty() && s[0] == prefix;
    }
    bool endsWith( std::string_view s, std::string_view suffix ) {
        return s.size() >= suffix.size() && std::equal(suffix.rbegin(), suffix.rend(), s.rbegin());
    }
    bool endsWith( std::string_view s, char suffix ) {
        return !s.empty() && s[s.size()-1] == suffix;
    }
    bool contains( std::string_view s, std::string_view infix ) {
        return s.find( infix ) != std::string::npos;
    }
    void toLowerInPlace( std::string& s ) {
        for ( char& c : s ) {
            c = toLower( c );
        }
    }
    std::string toLower( std::string_view s ) {
        std::string lc(s);
        toLowerInPlace( lc );
        return lc;
    }
    char toLower(char c) {
        return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    std::string trim( std::string const& str ) {
        static char const* whitespaceChars = "\n\r\t ";
        std::string::size_type start = str.find_first_not_of( whitespaceChars );
        std::string::size_type end = str.find_last_not_of( whitespaceChars );

        return start != std::string::npos ? str.substr( start, 1+end-start ) : std::string();
    }

    std::string_view trim(std::string_view ref) {
        const auto is_ws = [](char c) {
            return c == ' ' || c == '\t' || c == '\n' || c == '\r';
        };
        size_t real_begin = 0;
        while (real_begin < ref.size() && is_ws(ref[real_begin])) { ++real_begin; }
        size_t real_end = ref.size();
        while (real_end > real_begin && is_ws(ref[real_end - 1])) { --real_end; }

        return ref.substr(real_begin, real_end - real_begin);
    }

    bool replaceInPlace( std::string& str, std::string_view replaceThis, std::string_view withThis ) {
        std::size_t i = str.find( replaceThis );
        if (i == std::string::npos) {
            return false;
        }
        std::size_t copyBegin = 0;
        std::string origStr = CATCH_MOVE(str);
        str.clear();
        // There is at least one replacement, so reserve with the best guess
        // we can make without actually counting the number of occurrences.
        str.reserve(origStr.size() - replaceThis.size() + withThis.size());
        do {
            str.append(origStr, copyBegin, i-copyBegin );
            str += withThis;
            copyBegin = i + replaceThis.size();
            if( copyBegin < origStr.size() )
                i = origStr.find( replaceThis, copyBegin );
            else
                i = std::string::npos;
        } while( i != std::string::npos );
        if ( copyBegin < origStr.size() ) {
            str.append(origStr, copyBegin, origStr.size() );
        }
        return true;
    }

    std::vector<std::string_view> splitStringRef( std::string_view str, char delimiter ) {
        std::vector<std::string_view> subStrings;
        std::size_t start = 0;
        for(std::size_t pos = 0; pos < str.size(); ++pos ) {
            if( str[pos] == delimiter ) {
                if( pos - start > 1 )
                    subStrings.push_back( str.substr( start, pos-start ) );
                start = pos+1;
            }
        }
        if( start < str.size() )
            subStrings.push_back( str.substr( start, str.size()-start ) );
        return subStrings;
    }

    std::ostream& operator << ( std::ostream& os, pluralise const& pluraliser ) {
        os << pluraliser.m_count << ' ' << pluraliser.m_label;
        if( pluraliser.m_count != 1 )
            os << 's';
        return os;
    }

}
