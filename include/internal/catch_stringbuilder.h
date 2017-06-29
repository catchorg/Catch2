/*
 *  Copyright 2016 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef CATCH_STRINGBUILDER_H_INCLUDED
#define CATCH_STRINGBUILDER_H_INCLUDED

#include "catch_stringref.h"

namespace Catch {

    class String;
    class StringData;

    /// A mutable container for string data
    /// Use to build up strings before transferring to an immutable String.
    /// Construct the String using the rvalue reference constructor (which
    /// will usually involve std::move-ing the StringBuilder). This will transfer
    /// The underlying buffer without any extra allocations or ref counts.
    class StringBuilder {
        friend class String;
    public:
        using size_type = unsigned long;
        
        StringBuilder();
        StringBuilder( size_type initialCapacity );
        StringBuilder( StringRef const& str, size_type initialCapacity );
        StringBuilder( StringBuilder const& other, size_type initialCapacity );
        StringBuilder( StringBuilder&& other ) noexcept;
        StringBuilder( String&& other );
        StringBuilder( String const& other );
        ~StringBuilder() noexcept;

        void swap( StringBuilder& other ) noexcept;

        auto size() const noexcept -> size_type;
        auto capacity() const noexcept -> size_type;

        /// Grows the buffer to exactly the capacity requested, or
        /// does nothing if it is already at least as big
        void reserve(size_type capacity);

        /// Grows the buffer exponentially (from a baseline of 32 bytes)
        /// until it is at least as large as the requested capacity -
        /// or does nothing if already large enough
        void reserveExponential(size_type capacity);

        /// Writes the string at the current insertion point then moves
        /// the insertion point forward by the string length.
        /// If the buffer needs to grow to accomodate the string it does so
        /// using the exponential strategy
        void append( StringRef const& str );
        
        friend auto operator << ( StringBuilder& sb, StringRef sr ) -> StringBuilder&;
        friend auto operator << ( StringBuilder&& sb, StringRef sr ) -> StringBuilder&&;

        /// Writes the contents of the string ref into the buffer at
        /// the indexed location.
        /// The bounds are not checked! Use append() to just add to the
        /// end of the buffer, extending it if the capacity is not enough.
        void writeTo( size_type index, StringRef const& str );

    private:
        size_type m_size = 0;
        StringData* m_data;
    };
    
} // namespace Catch

#endif // CATCH_STRINGBUILDER_H_INCLUDED
