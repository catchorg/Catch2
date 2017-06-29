/*
 *  Copyright 2016 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_stringbuilder.h"
#include "catch_stringref.h"
#include "catch_stringdata.h"
#include "catch_string.h"

#include <cstring>
#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace Catch {

    static const StringBuilder::size_type s_minimumCapacity = 32;

    StringBuilder::StringBuilder()
    :   m_data( StringData::getEmpty() )
    {}
    
    StringBuilder::StringBuilder( size_type initialCapacity )
    :   m_size( 0 ),
        m_data( StringData::create( StringRef(), initialCapacity ) )
    {}
    StringBuilder::StringBuilder( StringRef const& str, size_type initialCapacity )
    :   m_size( str.size() ),
        m_data( StringData::create( str, initialCapacity ) )
    {}
    StringBuilder::StringBuilder( StringBuilder const& other, size_type initialCapacity )
    :   StringBuilder( StringRef( other.m_data->chars, other.m_size ), initialCapacity )
    {}
    StringBuilder::StringBuilder( StringBuilder&& other ) noexcept
    :   StringBuilder()
    {
        swap( other );
    }
    StringBuilder::StringBuilder( String&& str )
    :   m_size( str.size() ),
        m_data( StringData::getEmpty() )
    {
        if( str.m_data->isUniquelyOwned() )
        {
            std::swap( m_data, const_cast<StringData*&>( str.m_data ) );
        }
        else
        {
            size_type initialCapacity = std::min( s_minimumCapacity, m_size );
            m_data = StringData::create( str, initialCapacity );
        }
    }
    StringBuilder::StringBuilder( String const& other )
    : StringBuilder( StringRef( other ), std::min( s_minimumCapacity, other.size() ) )
    {}

    StringBuilder::~StringBuilder() noexcept {
        m_data->release();
    }

    auto StringBuilder::size() const noexcept -> size_type {
        return m_size;
    }
    
    void StringBuilder::swap( StringBuilder& other ) noexcept {
        std::swap( m_size, other.m_size );
        std::swap( m_data, other.m_data );
    }
    void StringBuilder::reserve( size_type minimumCapacity ) {
        if( minimumCapacity > capacity() ) {
            StringBuilder temp( *this, minimumCapacity );
            swap( temp );
        }
    }
    void StringBuilder::reserveExponential( size_type minimumCapacity ) {
        if( minimumCapacity > capacity() ) {
            size_type candidateCapacity = capacity() < s_minimumCapacity ? s_minimumCapacity : capacity()*2;
            while( candidateCapacity < minimumCapacity )
                candidateCapacity = candidateCapacity * 3/2; // grow factor of 1.5
            StringBuilder temp( *this, candidateCapacity );
            swap( temp );
        }
    }
    auto StringBuilder::capacity() const noexcept -> size_type {
        return m_data->size;
    }
    void StringBuilder::writeTo( size_type index, StringRef const& str ) {
        assert( index + str.size() < capacity() );
        if( str.size() > 0 )
            std::memcpy( m_data->chars+index, str.data(), str.size() );
    }
    void StringBuilder::append( StringRef const& str ) {
        reserveExponential( m_size + str.size() + 1 );
        writeTo( m_size, str );
        m_size += str.size();
        m_data->chars[m_size] = '\0';
    }

    auto operator << ( StringBuilder& sb, StringRef sr ) -> StringBuilder& {
        sb.append( sr );
        return sb;
    }
    auto operator << ( StringBuilder&& sb, StringRef sr ) -> StringBuilder&& {
        sb.append( sr );
        return std::move( sb );
    }
    
} // namespace Catch
