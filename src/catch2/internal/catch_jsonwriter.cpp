
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// Note: swapping these two includes around causes MSVC to error out
//       while in /permissive- mode. No, I don't know why.
//       Tested on VS 2019, 18.{3, 4}.x
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_jsonwriter.hpp>

namespace {
    void indent( std::ostream& os, std::uint64_t level ) {
        for ( std::uint64_t i = 0; i < level; ++i ) {
            os << "  ";
        }
    }
    void appendCommaNewline( std::ostream& os,
                             bool& should_comma,
                             std::uint64_t level ) {
        if ( should_comma ) { os << ','; }
        should_comma = true;
        os << '\n';
        indent( os, level );
    }
} // namespace

namespace Catch {
    JsonObjectWriter::JsonObjectWriter( std::ostream& os ):
        JsonObjectWriter{ os, 0 } {}

    JsonObjectWriter::JsonObjectWriter( std::ostream& os,
                                        std::uint64_t indent_level ):
        m_os{ os }, m_indent_level{ indent_level } {
        m_os << "{";
    }

    JsonObjectWriter::~JsonObjectWriter() {
        m_os << '\n';
        indent( m_os, m_indent_level );
        m_os << '}';
    }

    JsonValueWriter JsonObjectWriter::write( std::string const& key ) {
        appendCommaNewline( m_os, m_should_comma, m_indent_level + 1 );

        m_os << '"' << key << '"' << ": ";
        return JsonValueWriter{ m_os, m_indent_level + 1 };
    }

    JsonArrayWriter::JsonArrayWriter( std::ostream& os ):
        JsonArrayWriter{ os, 0 } {}
    JsonArrayWriter::JsonArrayWriter( std::ostream& os,
                                      std::uint64_t indent_level ):
        m_os{ os }, m_indent_level{ indent_level } {
        m_os << "[";
    }
    JsonArrayWriter::~JsonArrayWriter() {
        m_os << '\n';
        indent( m_os, m_indent_level );
        m_os << ']';
    }

    JsonObjectWriter JsonArrayWriter::writeObject() {
        appendCommaNewline( m_os, m_should_comma, m_indent_level + 1 );
        return JsonObjectWriter{ m_os, m_indent_level + 1 };
    }

    JsonArrayWriter JsonArrayWriter::writeArray() {
        appendCommaNewline( m_os, m_should_comma, m_indent_level + 1 );
        return JsonArrayWriter{ m_os, m_indent_level + 1 };
    }

    template <typename T>
    JsonArrayWriter& JsonArrayWriter::writeImpl( T const& value ) {
        appendCommaNewline( m_os, m_should_comma, m_indent_level + 1 );
        JsonValueWriter{ m_os }.write( value );

        return *this;
    }

    JsonArrayWriter& JsonArrayWriter::write( int value ) {
        return writeImpl( value );
    }
    JsonArrayWriter& JsonArrayWriter::write( double value ) {
        return writeImpl( value );
    }
    JsonArrayWriter& JsonArrayWriter::write( bool value ) {
        return writeImpl( value );
    }
    JsonArrayWriter& JsonArrayWriter::write( StringRef value ) {
        return writeImpl( value );
    }
    JsonArrayWriter& JsonArrayWriter::write( char const* value ) {
        return writeImpl( value );
    }

    JsonValueWriter::JsonValueWriter( std::ostream& os ):
        JsonValueWriter{ os, 0 } {}

    JsonValueWriter::JsonValueWriter( std::ostream& os,
                                      std::uint64_t indent_level ):
        m_os{ os }, m_indent_level{ indent_level } {}

    template <typename T>
    void JsonValueWriter::writeImpl( T const& value, bool quote_value ) {
        if ( quote_value ) { m_os << '"'; }
        m_os << value;
        if ( quote_value ) { m_os << '"'; }
    }

    JsonObjectWriter JsonValueWriter::writeObject() && {
        return JsonObjectWriter{ m_os, m_indent_level };
    }

    JsonArrayWriter JsonValueWriter::writeArray() && {
        return JsonArrayWriter{ m_os, m_indent_level };
    }

    void JsonValueWriter::write( int value ) && { writeImpl( value, false ); }
    void JsonValueWriter::write( double value ) && {
        writeImpl( value, false );
    }
    void JsonValueWriter::write( bool value ) && {
        writeImpl( value ? "true" : "false", false );
    }
    void JsonValueWriter::write( StringRef value ) && {
        writeImpl( value, true );
    }
    void JsonValueWriter::write( char const* value ) && {
        writeImpl( value, true );
    }

} // namespace Catch
