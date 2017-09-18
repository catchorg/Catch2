/*
 *  Copyright 2016 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef CATCH_STRINGREF_H_INCLUDED
#define CATCH_STRINGREF_H_INCLUDED

#include <cstddef>
#include <string>
#include <iosfwd>

namespace Catch {
    
    class StringData;

    /// A non-owning string class (similar to the forthcoming std::string_view)
    /// Note that, because a StringRef may be a substring of another string,
    /// it may not be null terminated. c_str() must return a null terminated
    /// string, however, and so the StringRef will internally take ownership
    /// (taking a copy), if necessary. In theory this ownership is not externally
    /// visible - but it does mean (substring) StringRefs should not be shared between
    /// threads.
    class StringRef {
        friend struct StringRefTestAccess;

        using size_type = std::size_t;
        
        char const* m_start;
        size_type m_size;
        
        char* m_data = nullptr;
        
        void takeOwnership();
        
    public: // construction/ assignment
        StringRef() noexcept;
        StringRef( StringRef const& other ) noexcept;
        StringRef( StringRef&& other ) noexcept;
        StringRef( char const* rawChars ) noexcept;
        StringRef( char const* rawChars, size_type size ) noexcept;
        StringRef( std::string const& stdString ) noexcept;
        ~StringRef() noexcept;
        
        auto operator = ( StringRef other ) noexcept -> StringRef&;
        operator std::string() const;

        void swap( StringRef& other ) noexcept;
        
    public: // operators
        auto operator == ( StringRef const& other ) const noexcept -> bool;
        auto operator != ( StringRef const& other ) const noexcept -> bool;
        
        auto operator[] ( size_type index ) const noexcept -> char;
        
    public: // named queries
        auto empty() const noexcept -> bool;
        auto size() const noexcept -> size_type;
        auto numberOfCharacters() const noexcept -> size_type;
        auto c_str() const -> char const*;
        
    public: // substrings and searches
        auto substr( size_type start, size_type size ) const noexcept -> StringRef;

    private: // ownership queries - may not be consistent between calls
        auto isOwned() const noexcept -> bool;
        auto isSubstring() const noexcept -> bool;
        auto data() const noexcept -> char const*;
    };

    auto operator + ( StringRef const& lhs, StringRef const& rhs ) -> std::string;
    auto operator + ( StringRef const& lhs, char const* rhs ) -> std::string;
    auto operator + ( char const* lhs, StringRef const& rhs ) -> std::string;

    auto operator << ( std::ostream& os, StringRef const& sr ) -> std::ostream&;

} // namespace Catch

#endif // CATCH_STRINGREF_H_INCLUDED
