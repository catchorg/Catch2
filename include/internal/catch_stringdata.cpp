/*
 *  Copyright 2016 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_stringdata.h"
#include "catch_stringref.h"

#include <new>
#include <cstring>
#include <cassert>

namespace Catch {
    
    auto StringData::getEmpty() -> StringData* {
        static StringData s_empty( 0 );
        return &s_empty;
    }
    auto StringData::create( StringRef const& stringRef ) -> StringData* {
        return create( stringRef, stringRef.size() );
    }
    auto StringData::create( StringRef const& stringRef, size_t capacity ) -> StringData* {
        if( capacity == 0 ) {
            return getEmpty();
        }
        else {
            assert( stringRef.size() <= capacity );
            auto bufferLen = sizeof(StringData)+capacity;
            void* buffer = new char[bufferLen];
            
            return new(buffer) StringData( stringRef, capacity );
        }
    }
    StringData::StringData( unsigned int initialRef )
    :   m_refs( initialRef ),
        size( 0 )
    {}
    StringData::StringData( StringRef const& stringRef, size_t capacity )
    :   m_refs( 1 ),
        size( capacity)
    {
        std::memcpy( chars, stringRef.data(), stringRef.size() );
        chars[stringRef.size() ] = 0;
    }
    
} // namespace Catch