/*
 *  Copyright 2016 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_stringref.h"
#include "catch_stringbuilder.h"
#include "catch_string.h"
#include "catch_stringdata.h"

#include <cstring>
#include <ostream>
#include <cassert>

namespace Catch {

    auto getEmptyStringRef() -> StringRef {
        static StringRef s_emptyStringRef("");
        return s_emptyStringRef;
    }
    
    StringRef::StringRef() noexcept
    :   StringRef( getEmptyStringRef() )
    {}
    
    StringRef::StringRef( StringRef const& other ) noexcept
    :   m_start( other.m_start ),
        m_size( other.m_size ),
        m_data( other.m_data )
    {
        if( m_data )
            m_data->addRef();
    }
    
    StringRef::StringRef( StringRef&& other ) noexcept
    :   m_start( other.m_start ),
        m_size( other.m_size ),
        m_data( other.m_data )
    {
        other.m_data = nullptr;
    }
    
    StringRef::StringRef( char const* rawChars ) noexcept
    :   m_start( rawChars ),
        m_size( static_cast<size_type>( std::strlen( rawChars ) ) )
    {
        assert( rawChars != nullptr );
    }
    
    StringRef::StringRef( char const* rawChars, size_type size ) noexcept
    :   m_start( rawChars ),
        m_size( size )
    {
        size_type rawSize = rawChars == nullptr ? 0 : static_cast<size_type>( std::strlen( rawChars ) );
        if( rawSize < size )
            m_size = rawSize;
    }
    
    StringRef::StringRef( String const& other ) noexcept
    :   m_start( other.c_str() ),
        m_size( other.size() ),
        m_data( nullptr )
    {}
    
    StringRef::StringRef( String&& str ) noexcept
    :   m_start( str.c_str() ),
        m_size( str.size() ),
        m_data( str.m_data )
    {
        str.m_data = StringData::getEmpty();
    }
    
    StringRef::~StringRef() noexcept {
        if( isOwned() )
            m_data->release();
    }
    
    auto StringRef::operator = ( StringRef other ) noexcept -> StringRef& {
        swap( other );
        return *this;
    }

    void StringRef::swap( StringRef& other ) noexcept {
        std::swap( m_start, other.m_start );
        std::swap( m_size, other.m_size );
        std::swap( m_data, other.m_data );
    }
    
    auto StringRef::c_str() const -> char const* {
        if( isSubstring() )
           const_cast<StringRef*>( this )->takeOwnership();
        return m_start;
    }
    auto StringRef::data() const noexcept -> char const* {
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
            StringRef temp = String( *this );
            swap( temp );
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
        
    auto operator + ( StringRef const& lhs, StringRef const& rhs ) -> String {
        StringBuilder buf;
        buf.reserve( lhs.size() + rhs.size() );
        buf.append( lhs );
        buf.append( rhs );
        return String( std::move( buf ) );
    }
    auto operator + ( StringRef const& lhs, const char* rhs ) -> String {
        return lhs + StringRef( rhs );
    }
    auto operator + ( char const* lhs, StringRef const& rhs ) -> String {
        return StringRef( lhs ) + rhs;
    }

    std::ostream& operator << ( std::ostream& os, StringRef const& str ) {
        return os << str.c_str();
    }
        
} // namespace Catch
