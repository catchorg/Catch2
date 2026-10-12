
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_textflow.hpp>

#include <algorithm>
#include <cstring>
#include <ostream>

namespace {
    bool isWhitespace( char c ) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    bool isBreakableBefore( char c ) {
        static const char chars[] = "[({<|";
        return std::memchr( chars, c, sizeof( chars ) - 1 ) != nullptr;
    }

    bool isBreakableAfter( char c ) {
        static const char chars[] = "])}>.,:;*+-=&/\\";
        return std::memchr( chars, c, sizeof( chars ) - 1 ) != nullptr;
    }

    bool isUtf8ContinuationByte( char c ) {
        return ( static_cast<unsigned char>( c ) & 0xC0 ) == 0x80;
    }

} // namespace

namespace Catch {
    namespace TextFlow {
        void AnsiSkippingString::preprocessString() {
            // If there are no potential escapes, we can use fast path
            if ( m_string.find( '\033' ) == std::string::npos ) {
                size_t size = 0;
                for ( const char c : m_string ) {
                    size += !isUtf8ContinuationByte( c );
                }
                m_size = size;
                return;
            }
            for ( auto it = m_string.begin(); it != m_string.end(); ) {
                // try to read through an ansi sequence
                while ( it != m_string.end() && *it == '\033' &&
                        it + 1 != m_string.end() && *( it + 1 ) == '[' ) {
                    auto cursor = it + 2;
                    while ( cursor != m_string.end() &&
                            ( isdigit( *cursor ) || *cursor == ';' ) ) {
                        ++cursor;
                    }
                    if ( cursor == m_string.end() || *cursor != 'm' ) {
                        break;
                    }
                    // record the escape sequence's byte range
                    m_escapes.push_back(
                        { std::distance( m_string.begin(), it ),
                          std::distance( m_string.begin(), cursor + 1 ) } );
                    // if we've read an ansi sequence, set the iterator and
                    // return to the top of the loop
                    it = cursor + 1;
                }
                if ( it != m_string.end() ) {
                    ++m_size;
                    ++it;
                    // Skip UTF-8 continuation bytes
                    while ( it != m_string.end() &&
                            isUtf8ContinuationByte( *it ) ) {
                        ++it;
                    }
                }
            }
        }

        AnsiSkippingString::AnsiSkippingString( std::string const& text ):
            m_string( text ) {
            preprocessString();
        }

        AnsiSkippingString::AnsiSkippingString( std::string&& text ):
            m_string( CATCH_MOVE( text ) ) {
            preprocessString();
        }

        AnsiSkippingString::const_iterator AnsiSkippingString::begin() const {
            return const_iterator( *this );
        }

        AnsiSkippingString::const_iterator AnsiSkippingString::end() const {
            return const_iterator( *this, const_iterator::EndTag{} );
        }

        std::string AnsiSkippingString::substring( const_iterator first,
                                                   const_iterator last ) const {
            std::string ret;
            appendSubstringTo( ret, first, last );
            return ret;
        }

        void
        AnsiSkippingString::appendSubstringTo( std::string& out,
                                               const_iterator first,
                                               const_iterator last ) const {
            // There's one caveat here to an otherwise simple substring: when
            // making a begin iterator we might have skipped ansi sequences at
            // the start. If `first` here is a begin iterator, skipped over
            // initial ansi sequences, we'll use the true beginning of the
            // string.
            if ( hasEscapes() && first == begin() ) {
                out.append( m_string.begin(), last.m_it );
            } else {
                out.append( first.m_it, last.m_it );
            }
        }

        void AnsiSkippingString::const_iterator::jumpForwardOverEscapes() {
            // Escapes can only start with '\033'
            if ( m_it == m_string->m_string.end() || *m_it != '\033' ) {
                return;
            }
            // If we are at '\033', check if we are at an actual escape
            auto current_pos = std::distance( m_string->m_string.begin(), m_it );

            auto const& escapes = m_string->m_escapes;
            auto candidate =
                std::lower_bound( escapes.begin(),
                                  escapes.end(),
                                  current_pos,
                                  []( AnsiSkippingString::EscapeRange const& r,
                                      std::ptrdiff_t o ) { return r.start < o; } );
            // Escapes can be consecutive, we need to jump over all of them
            while ( candidate != escapes.end() && candidate->start == current_pos ) {
                current_pos = candidate->end;
                ++candidate;
            }
            m_it = m_string->m_string.begin() + current_pos;
        }

        void AnsiSkippingString::const_iterator::jumpBackOverEscapes() {
            auto strBegin = m_string->m_string.begin();
            // Escapes only end with 'm'
            while ( *m_it == 'm' ) {
                // If we are at 'm', check if we are at an actual escape
                const auto current_pos = std::distance( strBegin, m_it );

                auto const& escapes = m_string->m_escapes;
                auto range = std::lower_bound(
                    escapes.begin(),
                    escapes.end(),
                    current_pos + 1, // to half-open range
                    []( AnsiSkippingString::EscapeRange const& r,
                        std::ptrdiff_t o ) { return r.end < o; } );
                if ( range == escapes.end() || range->end != current_pos + 1 ) {
                    break;
                }
                assert( range->start != 0 &&
                        "Trying to go back with begin iterator" );
                m_it = strBegin + range->start - 1;
            }
        }


        void AnsiSkippingString::const_iterator::advance() {
            auto strEnd = m_string->m_string.end();
            assert( m_it != strEnd );
            m_it++;
            // Skip UTF-8 continuation bytes
            while ( m_it != strEnd &&
                    isUtf8ContinuationByte( *m_it ) ) {
                m_it++;
            }
            if ( m_string->hasEscapes() ) { jumpForwardOverEscapes(); }
        }

        void AnsiSkippingString::const_iterator::unadvance() {
            auto strBegin = m_string->m_string.begin();
            assert( m_it != strBegin );
            m_it--;
            if ( m_string->hasEscapes() ) { jumpBackOverEscapes(); }
            // Skip back over UTF-8 continuation bytes to the leading byte
            while ( m_it != strBegin &&
                    isUtf8ContinuationByte( *m_it ) ) {
                m_it--;
            }
        }

        static bool isBoundary( AnsiSkippingString const& line,
                                AnsiSkippingString::const_iterator it ) {
            return it == line.end() ||
                   ( isWhitespace( *it ) &&
                     !isWhitespace( *it.oneBefore() ) ) ||
                   isBreakableBefore( *it ) ||
                   isBreakableAfter( *it.oneBefore() );
        }

        void Column::const_iterator::calcLength() {
            m_addHyphen = false;
            m_parsedTo = m_lineStart;
            AnsiSkippingString const& current_line = m_column.m_string;

            if ( m_parsedTo == current_line.end() ) {
                m_lineEnd = m_parsedTo;
                return;
            }

            assert( m_lineStart != current_line.end() );
            if ( *m_lineStart == '\n' ) { ++m_parsedTo; }

            const auto maxLineLength = m_column.m_width - indentSize();
            std::size_t lineLength = 0;
            while ( m_parsedTo != current_line.end() &&
                    lineLength < maxLineLength && *m_parsedTo != '\n' ) {
                ++m_parsedTo;
                ++lineLength;
            }

            // If we encountered a newline before the column is filled,
            // then we linebreak at the newline and consider this line
            // finished.
            if ( lineLength < maxLineLength ) {
                m_lineEnd = m_parsedTo;
            } else {
                // Look for a natural linebreak boundary in the column
                // (We look from the end, so that the first found boundary is
                // the right one)
                m_lineEnd = m_parsedTo;
                while ( lineLength > 0 &&
                        !isBoundary( current_line, m_lineEnd ) ) {
                    --lineLength;
                    --m_lineEnd;
                }
                while ( lineLength > 0 &&
                        isWhitespace( *m_lineEnd.oneBefore() ) ) {
                    --lineLength;
                    --m_lineEnd;
                }

                // If we found one, then that is where we linebreak, otherwise
                // we have to split text with a hyphen
                if ( lineLength == 0 ) {
                    m_addHyphen = true;
                    m_lineEnd = m_parsedTo.oneBefore();
                }
            }
        }

        size_t Column::const_iterator::indentSize() const {
            auto initial = m_lineStart == m_column.m_string.begin()
                               ? m_column.m_initialIndent
                               : std::string::npos;
            return initial == std::string::npos ? m_column.m_indent : initial;
        }

        std::string Column::const_iterator::addIndentAndSuffix(
            AnsiSkippingString::const_iterator start,
            AnsiSkippingString::const_iterator end ) const {
            std::string ret;
            const auto desired_indent = indentSize();
            ret.append( desired_indent, ' ' );
            m_column.m_string.appendSubstringTo( ret, start, end );
            if ( m_addHyphen ) { ret.push_back( '-' ); }

            return ret;
        }

        Column::const_iterator::const_iterator( Column const& column ):
            m_column( column ),
            m_lineStart( column.m_string.begin() ),
            m_lineEnd( column.m_string.begin() ),
            m_parsedTo( column.m_string.begin() ) {
            assert( m_column.m_width > m_column.m_indent );
            assert( m_column.m_initialIndent == std::string::npos ||
                    m_column.m_width > m_column.m_initialIndent );
            calcLength();
            if ( m_lineStart == m_lineEnd ) {
                m_lineStart = m_column.m_string.end();
            }
        }

        std::string Column::const_iterator::operator*() const {
            assert( m_lineStart <= m_parsedTo );
            return addIndentAndSuffix( m_lineStart, m_lineEnd );
        }

        Column::const_iterator& Column::const_iterator::operator++() {
            m_lineStart = m_lineEnd;
            AnsiSkippingString const& current_line = m_column.m_string;
            if ( m_lineStart != current_line.end() && *m_lineStart == '\n' ) {
                m_lineStart++;
            } else {
                while ( m_lineStart != current_line.end() &&
                        isWhitespace( *m_lineStart ) ) {
                    ++m_lineStart;
                }
            }

            if ( m_lineStart != current_line.end() ) { calcLength(); }
            return *this;
        }

        Column::const_iterator Column::const_iterator::operator++( int ) {
            const_iterator prev( *this );
            operator++();
            return prev;
        }

        std::ostream& operator<<( std::ostream& os, Column const& col ) {
            bool first = true;
            for ( auto line : col ) {
                if ( first ) {
                    first = false;
                } else {
                    os << '\n';
                }
                os << line;
            }
            return os;
        }

        Column Spacer( size_t spaceWidth ) {
            Column ret{ "" };
            ret.width( spaceWidth );
            return ret;
        }

        Columns::iterator::iterator( Columns const& columns, EndTag ):
            m_columns( columns.m_columns ), m_activeIterators( 0 ) {

            m_iterators.reserve( m_columns.size() );
            for ( auto const& col : m_columns ) {
                m_iterators.push_back( col.end() );
            }
        }

        Columns::iterator::iterator( Columns const& columns ):
            m_columns( columns.m_columns ),
            m_activeIterators( m_columns.size() ) {

            m_iterators.reserve( m_columns.size() );
            for ( auto const& col : m_columns ) {
                m_iterators.push_back( col.begin() );
            }
        }

        std::string Columns::iterator::operator*() const {
            std::string row, padding;

            for ( size_t i = 0; i < m_columns.size(); ++i ) {
                const auto width = m_columns[i].width();
                if ( m_iterators[i] != m_columns[i].end() ) {
                    std::string col = *m_iterators[i];
                    row += padding;
                    row += col;

                    padding.clear();
                    if ( col.size() < width ) {
                        padding.append( width - col.size(), ' ' );
                    }
                } else {
                    padding.append( width, ' ' );
                }
            }
            return row;
        }

        Columns::iterator& Columns::iterator::operator++() {
            for ( size_t i = 0; i < m_columns.size(); ++i ) {
                if ( m_iterators[i] != m_columns[i].end() ) {
                    ++m_iterators[i];
                }
            }
            return *this;
        }

        Columns::iterator Columns::iterator::operator++( int ) {
            iterator prev( *this );
            operator++();
            return prev;
        }

        std::ostream& operator<<( std::ostream& os, Columns const& cols ) {
            bool first = true;
            for ( auto line : cols ) {
                if ( first ) {
                    first = false;
                } else {
                    os << '\n';
                }
                os << line;
            }
            return os;
        }

        Columns operator+( Column const& lhs, Column const& rhs ) {
            Columns cols;
            cols += lhs;
            cols += rhs;
            return cols;
        }
        Columns operator+( Column&& lhs, Column&& rhs ) {
            Columns cols;
            cols += CATCH_MOVE( lhs );
            cols += CATCH_MOVE( rhs );
            return cols;
        }

        Columns& operator+=( Columns& lhs, Column const& rhs ) {
            lhs.m_columns.push_back( rhs );
            return lhs;
        }
        Columns& operator+=( Columns& lhs, Column&& rhs ) {
            lhs.m_columns.push_back( CATCH_MOVE( rhs ) );
            return lhs;
        }
        Columns operator+( Columns const& lhs, Column const& rhs ) {
            auto combined( lhs );
            combined += rhs;
            return combined;
        }
        Columns operator+( Columns&& lhs, Column&& rhs ) {
            lhs += CATCH_MOVE( rhs );
            return CATCH_MOVE( lhs );
        }

    } // namespace TextFlow
} // namespace Catch
