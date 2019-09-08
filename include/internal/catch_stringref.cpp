/*
 *  Copyright 2016 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif

#include "catch_stringref.h"

#include <ostream>
#include <cstring>
#include <cstdint>

namespace Catch {
    StringRef::StringRef( char const* rawChars ) noexcept
    : StringRef( rawChars, static_cast<StringRef::size_type>(std::strlen(rawChars) ) )
    {}

    void StringRef::swap( StringRef& other ) noexcept {
        std::swap( m_start, other.m_start );
        std::swap( m_size, other.m_size );
        std::swap( m_data, other.m_data );
    }

    auto StringRef::c_str() const -> char const* {
        if( !isSubstring() )
            return m_start;

        const_cast<StringRef *>( this )->takeOwnership();
        return m_data;
    }
    auto StringRef::currentData() const noexcept -> char const* {
        return m_start;
    }

    auto StringRef::isOwned() const noexcept -> bool {
        return m_data != nullptr;
    }
    auto StringRef::isSubstring() const noexcept -> bool {
        return m_start[m_size] != '\0';
    }

    void StringRef::takeOwnership() {
        if( !isOwned() ) {
            m_data = new char[m_size+1];
            memcpy( m_data, m_start, m_size );
            m_data[m_size] = '\0';
        }
    }
    auto StringRef::substr( size_type start, size_type size ) const noexcept -> StringRef {
        if( start < m_size )
            return StringRef( m_start+start, size );
        else
            return StringRef();
    }
    auto StringRef::operator == ( StringRef const& other ) const noexcept -> bool {
        return
            size() == other.size() &&
            (std::strncmp( m_start, other.m_start, size() ) == 0);
    }
    auto StringRef::operator != ( StringRef const& other ) const noexcept -> bool {
        return !operator==( other );
    }

    auto operator << ( std::ostream& os, StringRef const& str ) -> std::ostream& {
        return os.write(str.currentData(), str.size());
    }

    auto operator+=( std::string& lhs, StringRef const& rhs ) -> std::string& {
        lhs.append(rhs.currentData(), rhs.size());
        return lhs;
    }

} // namespace Catch

#if defined(__clang__)
#    pragma clang diagnostic pop
#endif
