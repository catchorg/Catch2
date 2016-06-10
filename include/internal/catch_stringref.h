/*
 *  Copyright 2016 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_STRINGREF_H_INCLUDED
#define TWOBLUECUBES_CATCH_STRINGREF_H_INCLUDED

#include "internal/catch_suppress_warnings.h"

namespace Catch {
    
    // A non-owning string class (similar to the forthcoming std::string_view)
    // Note that, because a StringRef may be a substring of another string,
    // it may not be null terminated. c_str() must return a null terminated
    // string, however, and so the StringRef will internally take ownership
    // (taking a copy), if necessary. This internal mutable is not externally
    // visible - but does mean StringRefs should not be shared between
    // threads.
    class StringRef {
    protected:
        enum class Ownership {
            FullStringRef,
            SubStringRef,
            FullStringOwned
        };
        
        using size_type = unsigned long;
        
        char const*  m_data;
        size_type m_size;
        Ownership m_ownership;
        
        static StringRef s_emptyStringRef;
        
        void takeOwnership();
        void deleteIfOwned();
        
        // Access for testing purposes only ////////////
        friend auto rawCharData( StringRef const& stringRef ) -> char const*;
        friend auto isOwned( StringRef const& stringRef ) -> bool;
        friend auto isSubstring( StringRef const& stringRef ) -> bool;
        ////////////////////////////////////////////////
        
    public: // construction/ assignment
        StringRef();
        StringRef( StringRef const& other );
        StringRef( StringRef&& other );
        StringRef( char const* rawChars );
        StringRef( char const* rawChars, size_type size );
        ~StringRef();
        
        auto operator = ( StringRef const& other ) -> StringRef&;
        auto operator = ( StringRef&& other ) -> StringRef&;
        
    public: // operators
        auto operator == ( StringRef const& other ) const -> bool;
        auto operator != ( StringRef const& other ) const -> bool;
        
        auto operator[] ( size_type index ) const -> char {
            return m_data[index];
        }
        
    public: // named queries
        auto empty() const -> bool {
            return m_size == 0;
        }
        auto size() const -> size_type {
            return m_size;
        }
        auto c_str() const -> char const*;
        
    public: // substrings and searches
        auto substr( size_type start, size_type size ) const -> StringRef;
    };

} // namespace Catch

#include "internal/catch_reenable_warnings.h"

#endif // TWOBLUECUBES_CATCH_STRINGREF_H_INCLUDED
