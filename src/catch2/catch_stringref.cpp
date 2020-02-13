/*
 *  Copyright 2016 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <catch2/catch_enforce.h>
#include <catch2/catch_stringref.h>

#include <algorithm>
#include <ostream>
#include <cstring>
#include <cstdint>

namespace Catch {
    StringRef::StringRef( char const* rawChars ) noexcept
    : StringRef( rawChars, static_cast<StringRef::size_type>(std::strlen(rawChars) ) )
    {}

    auto StringRef::c_str() const -> char const* {
        CATCH_ENFORCE(isNullTerminated(), "Called StringRef::c_str() on a non-null-terminated instance");
        return m_start;
    }

    auto StringRef::operator == ( StringRef const& other ) const noexcept -> bool {
        return m_size == other.m_size
            && (std::memcmp( m_start, other.m_start, m_size ) == 0);
    }

    bool StringRef::operator<(StringRef const& rhs) const noexcept {
        if (m_size < rhs.m_size) {
            return strncmp(m_start, rhs.m_start, m_size) <= 0;
        }
        return strncmp(m_start, rhs.m_start, rhs.m_size) < 0;
    }

    auto operator << ( std::ostream& os, StringRef const& str ) -> std::ostream& {
        return os.write(str.data(), str.size());
    }

    auto operator+=( std::string& lhs, StringRef const& rhs ) -> std::string& {
        lhs.append(rhs.data(), rhs.size());
        return lhs;
    }

} // namespace Catch
