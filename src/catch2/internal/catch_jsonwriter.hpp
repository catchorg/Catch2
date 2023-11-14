
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_JSONWRITER_HPP_INCLUDED
#define CATCH_JSONWRITER_HPP_INCLUDED

#include <catch2/internal/catch_reusable_string_stream.hpp>
#include <catch2/internal/catch_stringref.hpp>

#include <cstdint>
#include <sstream>

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
        JsonValueWriter( std::ostream& os );
        JsonValueWriter( std::ostream& os, std::uint64_t indent_level );

        JsonObjectWriter writeObject() &&;
        JsonArrayWriter writeArray() &&;

        template <typename T>
        void write( T const& value ) && {
            writeImpl( value, !std::is_arithmetic<T>::value );
        }

        void write( bool value ) &&;

    private:
        template <typename T>
        void writeImpl( T const& value, bool quote_value ) {
            if ( quote_value ) { m_os << '"'; }
            m_sstream << value;
            while ( true ) {
                char c = m_sstream.get();
                if ( m_sstream.eof() ) { break; }

                // see https://www.json.org/json-en.html, string definition for the escape list
                if ( c == '"' ) {
                    m_os << "\\\"";
                } else if ( c == '\\' ) {
                    m_os << "\\\\";
                // Note that while forward slash _can_ be escaped, it
                // does not have to be, if JSON is not further embedded
                // somewhere where forward slash is meaningful.
                } else if ( c == '\b' ) {
                    m_os << "\\b";
                } else if ( c == '\f' ) {
                    m_os << "\\f";
                } else if ( c == '\n' ) {
                    m_os << "\\n";
                } else if ( c == '\r' ) {
                    m_os << "\\r";
                } else if ( c == '\t') {
                    m_os << "\\t";
                } else {
                    m_os << c;
                }
            }
            if ( quote_value ) { m_os << '"'; }
        }

        std::ostream& m_os;
        std::stringstream m_sstream{};
        std::uint64_t m_indent_level;
    };

    class JsonObjectWriter {
    public:
        JsonObjectWriter( std::ostream& os );
        JsonObjectWriter( std::ostream& os, std::uint64_t indent_level );

        JsonObjectWriter( JsonObjectWriter&& source );
        JsonObjectWriter& operator=( JsonObjectWriter&& source ) = delete;

        ~JsonObjectWriter();

        JsonValueWriter write( std::string const& key );

    private:
        std::ostream& m_os;
        std::uint64_t m_indent_level;
        bool m_should_comma = false;
        bool m_active = true;
    };

    class JsonArrayWriter {
    public:
        JsonArrayWriter( std::ostream& os );
        JsonArrayWriter( std::ostream& os, std::uint64_t indent_level );

        JsonArrayWriter( JsonArrayWriter&& source );
        JsonArrayWriter& operator=( JsonArrayWriter&& source ) = delete;

        ~JsonArrayWriter();

        JsonObjectWriter writeObject();
        JsonArrayWriter writeArray();

        template <typename T>
        JsonArrayWriter& write( T const& value ) {
            return writeImpl( value );
        }

        JsonArrayWriter& write( bool value );

    private:
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
        bool m_active = true;
    };

} // namespace Catch

#endif // CATCH_JSONWRITER_HPP_INCLUDED
