/*
 *  Copyright 2016 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef CATCH_STRINGDATA_H_INCLUDED
#define CATCH_STRINGDATA_H_INCLUDED

#include <atomic>

namespace Catch {
    
    class StringRef;

    class StringData {
        mutable std::atomic<unsigned int> m_refs;
    public:
        unsigned int size;
        union {
            char chars[1];
        };

        auto isUniquelyOwned() const noexcept -> bool {
            return m_refs == 1;
        }
        static auto getEmpty() -> StringData*;
        static auto create( StringRef const& stringRef ) -> StringData*;
        static auto create( StringRef const& stringRef, unsigned long capacity ) -> StringData*;

        void addRef() const noexcept {
            if( m_refs > 0 )
                ++m_refs;
        }
        void release() const noexcept {
            unsigned int refs = m_refs;
            if( refs > 1 )
                --m_refs;
            else if( refs == 1 )
                delete[] reinterpret_cast<char const*>( this );
        }
    private:
        StringData( unsigned int initialRef = 1 );
        StringData( StringRef const& stringRef, unsigned long capacity );
        
        StringData( StringData const& ) = delete;
        StringData& operator=( StringData const& ) = delete;
    };

} // namespace Catch

#endif // CATCH_STRINGDATA_H_INCLUDED
