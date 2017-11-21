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
    public:
        using size_type = std::size_t;

    private:
        friend struct StringRefTestAccess;

        char const* m_start;
        size_type m_size;
        
        char* m_data = nullptr;
        
        void takeOwnership();

        static constexpr char const* const s_empty = "";
        
    public: // construction/ assignment
        StringRef() noexcept
        :   StringRef( s_empty, 0 )
        {}

        StringRef( StringRef const& other ) noexcept
        :   m_start( other.m_start ),
            m_size( other.m_size )
        {}

        StringRef( StringRef&& other ) noexcept
        :   m_start( other.m_start ),
            m_size( other.m_size ),
            m_data( other.m_data )
        {
            other.m_data = nullptr;
        }

        StringRef( char const* rawChars ) noexcept;

        StringRef( char const* rawChars, size_type size ) noexcept
        :   m_start( rawChars ),
            m_size( size )
        {}

        StringRef( std::string const& stdString ) noexcept
        :   m_start( stdString.c_str() ),
            m_size( stdString.size() )
        {}

        ~StringRef() noexcept {
            delete[] m_data;
        }

        auto operator = ( StringRef const &other ) noexcept -> StringRef& {
            delete[] m_data;
            m_data = nullptr;
            m_start = other.m_start;
            m_size = other.m_size;
            return *this;
        }

        operator std::string() const;

        void swap( StringRef& other ) noexcept;
        
    public: // operators
        auto operator == ( StringRef const& other ) const noexcept -> bool;
        auto operator != ( StringRef const& other ) const noexcept -> bool;
        
        auto operator[] ( size_type index ) const noexcept -> char;
        
    public: // named queries
        auto empty() const noexcept -> bool {
            return m_size == 0;
        }
        auto size() const noexcept -> size_type {
            return m_size;
        }

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

    inline auto operator "" _sr( char const* rawChars, std::size_t size ) noexcept -> StringRef {
        return StringRef( rawChars, size );
    }

} // namespace Catch

#endif // CATCH_STRINGREF_H_INCLUDED
