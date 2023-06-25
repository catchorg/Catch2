
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_JSONWRITER_HPP_INCLUDED
#define CATCH_JSONWRITER_HPP_INCLUDED

#include <catch2/internal/catch_reusable_string_stream.hpp>
#include <catch2/internal/catch_stringref.hpp>

#include <iosfwd>
#include <vector>

namespace Catch {
    class JsonObjectWriter;
    class JsonArrayWriter;

    struct JsonUtils {
        static void indent( std::ostream& os, std::uint64_t level );
        static void appendCommaNewline( std::ostream& os,
                                        bool& should_comma,
                                        std::uint64_t level );
    };

    class JsonValueWriter {
    public:
        friend JsonObjectWriter;
        friend JsonArrayWriter;

        JsonValueWriter( std::ostream& os );

        JsonObjectWriter writeObject() &&;
        JsonArrayWriter writeArray() &&;

        template <
            typename T,
            typename = typename std::enable_if_t<std::is_arithmetic<T>::value>>
        void write( T value ) && {
            writeImpl( value, false );
        }

        void write( bool value ) &&;
        void write( StringRef value ) &&;
        void write( char const* value ) &&;

    private:
        JsonValueWriter( std::ostream& os, std::uint64_t indent_level );

        template <typename T>
        void writeImpl( T const& value, bool quote_value ) {
            if ( quote_value ) { m_os << '"'; }
            m_os << value;
            if ( quote_value ) { m_os << '"'; }
        }

        std::ostream& m_os;
        std::uint64_t m_indent_level;
    };

    class JsonObjectWriter {
    public:
        friend JsonValueWriter;
        friend JsonArrayWriter;

        JsonObjectWriter( std::ostream& os );

        ~JsonObjectWriter();

        JsonValueWriter write( std::string const& key );

    private:
        JsonObjectWriter( std::ostream& os, std::uint64_t indent_level );

        std::ostream& m_os;
        std::uint64_t m_indent_level;
        bool m_should_comma = false;
    };

    class JsonArrayWriter {
    public:
        friend JsonValueWriter;
        friend JsonObjectWriter;

        JsonArrayWriter( std::ostream& os );

        JsonObjectWriter writeObject();
        JsonArrayWriter writeArray();

        template <
            typename T,
            typename = typename std::enable_if_t<std::is_arithmetic<T>::value>>
        JsonArrayWriter& write( T value ) {
            return writeImpl( value );
        }

        JsonArrayWriter& write( bool value );
        JsonArrayWriter& write( StringRef value );
        JsonArrayWriter& write( char const* value );

        ~JsonArrayWriter();

    private:
        JsonArrayWriter( std::ostream& os, std::uint64_t indent_level );

        template <typename T>
        JsonArrayWriter& writeImpl( T const& value ) {
            JsonUtils::appendCommaNewline(
                m_os, m_should_comma, m_indent_level + 1 );
            JsonValueWriter{ m_os }.write( value );

            return *this;
        }

        std::ostream& m_os;
        std::uint64_t m_indent_level;
        bool m_should_comma = false;
    };

    struct JsonWriter : JsonValueWriter {
        using JsonValueWriter::JsonValueWriter;
    };
} // namespace Catch

#endif // CATCH_JSONWRITER_HPP_INCLUDED
