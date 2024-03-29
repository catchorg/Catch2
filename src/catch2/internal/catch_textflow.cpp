
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

    bool isBoundary( std::string const& line, std::string::const_iterator it ) {
        return it == line.end() ||
               ( isWhitespace( *it ) && !isWhitespace( *(it - 1) ) ) ||
               isBreakableBefore( *it ) ||
               isBreakableAfter( *(it - 1) );
    }

} // namespace

namespace Catch {
    namespace TextFlow {

        void Column::const_iterator::calcLength() {
            m_addHyphen = false;
            m_parsedTo = m_lineStart;

            std::string const& current_line = m_column.m_string;
            if ( *m_lineStart == '\n' ) {
                ++m_parsedTo;
            }

            const auto maxLineLength = m_column.m_width - indentSize();
            std::size_t lineLength = 0;
            while ( m_parsedTo != current_line.end() && lineLength < maxLineLength && *m_parsedTo != '\n' ) {
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
                while ( lineLength > 0 && !isBoundary( current_line, m_lineEnd )) {
                    --lineLength;
                    --m_lineEnd;
                }
                while ( lineLength > 0 && isWhitespace( *(m_lineEnd - 1) ) ) {
                    --lineLength;
                    --m_lineEnd;
                }

                // If we found one, then that is where we linebreak
                if ( lineLength > 0 ) {
                } else {
                    // Otherwise we have to split text with a hyphen
                    m_addHyphen = true;
                    m_lineEnd = m_parsedTo - 1;
                }
            }
        }

        size_t Column::const_iterator::indentSize() const {
            auto initial =
                m_lineStart == m_column.m_string.begin() ? m_column.m_initialIndent : std::string::npos;
            return initial == std::string::npos ? m_column.m_indent : initial;
        }

        std::string
        Column::const_iterator::addIndentAndSuffix( std::string::const_iterator start,
                                              std::string::const_iterator end ) const {
            std::string ret;
            const auto desired_indent = indentSize();
            ret.reserve( desired_indent + (end - start) + m_addHyphen );
            ret.append( desired_indent, ' ' );
            ret.append( start, end );
            if ( m_addHyphen ) {
                ret.push_back( '-' );
            }

            return ret;
        }

        Column::const_iterator::const_iterator( Column const& column ): m_column( column ), m_lineStart(column.m_string.begin()), m_lineEnd(column.m_string.begin()) {
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
            m_lineStart = m_lineEnd; // FIXME
            std::string const& current_line = m_column.m_string;
            if ( m_lineStart < current_line.end() && *m_lineStart == '\n' ) {
                m_lineStart += 1;
            } else {
                while ( m_lineStart < current_line.end() && isWhitespace( *m_lineStart ) ) {
                    ++m_lineStart;
                }
            }

            if ( m_lineStart != current_line.end() ) {
                calcLength();
            }
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

        Columns operator+(Column const& lhs, Column const& rhs) {
            Columns cols;
            cols += lhs;
            cols += rhs;
            return cols;
        }
        Columns operator+(Column&& lhs, Column&& rhs) {
            Columns cols;
            cols += CATCH_MOVE( lhs );
            cols += CATCH_MOVE( rhs );
            return cols;
        }

        Columns& operator+=(Columns& lhs, Column const& rhs) {
            lhs.m_columns.push_back( rhs );
            return lhs;
        }
        Columns& operator+=(Columns& lhs, Column&& rhs) {
            lhs.m_columns.push_back( CATCH_MOVE(rhs) );
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
