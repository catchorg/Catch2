/*
 *  Copyright 2016 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef CATCH_STRING_H_INCLUDED
#define CATCH_STRING_H_INCLUDED

#include <cstddef>

namespace Catch {
    
    class StringData;
    class StringRef;
    class StringBuilder;

    /// An owning, ref-counted, immutable string type.
    /// The ref count (not visible here as it is defined in StringData) is atomic
    /// so instances should be safe to share across threads
    class String {
        friend class StringRef;
        friend class StringBuilder;

        StringData const* m_data = nullptr;
    public:
        using size_type = size_t;
        
        String();
        String( StringRef const& stringRef );
        String( char const* rawString );
        String( String const& other );
        String( String&& other ) noexcept;
        String( StringBuilder&& stringBuf );
        
        ~String() noexcept;
        
        auto operator = ( String const& other ) -> String&;
        
        auto operator == ( StringRef const& other ) const noexcept -> bool;
        auto operator == ( char const* other ) const noexcept -> bool;
        
        auto empty() const noexcept -> bool;
        auto size() const noexcept -> size_type;
        auto numberOfCharacters() const noexcept -> size_type;

        auto c_str() const noexcept -> char const*;
    };
    
} // namespace Catch

#endif // CATCH_STRING_H_INCLUDED
