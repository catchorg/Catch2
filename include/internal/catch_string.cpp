/*
 *  Copyright 2016 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_string.h"
#include "catch_stringref.h"
#include "catch_stringbuilder.h"

#include "catch_stringdata.h"

#include <ostream>

namespace Catch {
    
    String::String()
    : m_data( StringData::getEmpty() )
    {}
    
    String::String( StringRef const& stringRef )
    : m_data( StringData::create( stringRef ) )
    {}
    
    String::String( char const* rawString )
    : String( StringRef( rawString ) )
    {}

    String::String( String const& other )
    : m_data( other.m_data )
    {
        m_data->addRef();
    }
    String::String( String&& other ) noexcept
    : m_data( other.m_data )
    {
        other.m_data = StringData::getEmpty();
    }
    String::String( StringBuilder&& stringBuf )
    : m_data( stringBuf.m_data )
    {
        // const_cast is ok here because we are taking ownership
        const_cast<StringData*>( m_data )->size = stringBuf.size();
        stringBuf.m_data = StringData::getEmpty();
        stringBuf.m_size = 0;
    }

    
    String::~String() noexcept {
        m_data->release();
    }
    
    auto String::operator = ( String const& other ) -> String& {
        m_data = other.m_data;
        m_data->addRef();
        return *this;
    }
    
    
    auto String::empty() const noexcept -> bool {
        return m_data->size == 0;
    }
    auto String::size() const noexcept -> size_type {
        return m_data->size;
    }
    auto String::c_str() const noexcept -> char const* {
        return m_data->chars;
    }
    
    auto String::operator == ( StringRef const& other ) const noexcept -> bool {
        return other == StringRef( *this );
    }
    auto String::operator == ( char const* other ) const noexcept -> bool {
        return StringRef( other ) == StringRef( *this );
    }
    
    std::ostream& operator << ( std::ostream& os, String const& str ) {
        os << str.c_str();
        return os;
    }
    
} // namespace Catch
