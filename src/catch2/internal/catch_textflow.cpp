
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

    bool isBoundary( std::string const& line, size_t at ) {
        assert( at > 0 );
        assert( at <= line.size() );

        return at == line.size() ||
               ( isWhitespace( line[at] ) && !isWhitespace( line[at - 1] ) ) ||
               isBreakableBefore( line[at] ) ||
               isBreakableAfter( line[at - 1] );
    }

} // namespace

namespace Catch {
    namespace TextFlow {

        void Column::const_iterator::calcLength() {
            m_addHyphen = false;
            m_parsedTo = m_lineStart;

            std::string const& current_line = m_column.m_string;
            if ( current_line[m_lineStart] == '\n' ) {
                ++m_parsedTo;
            }

            const auto maxLineLength = m_column.m_width - indentSize();
            const auto maxParseTo = std::min(current_line.size(), m_lineStart + maxLineLength);
            while ( m_parsedTo < maxParseTo &&
                    current_line[m_parsedTo] != '\n' ) {
                ++m_parsedTo;
            }

            // If we encountered a newline before the column is filled,
            // then we linebreak at the newline and consider this line
            // finished.
            if ( m_parsedTo < m_lineStart + maxLineLength ) {
                m_lineLength = m_parsedTo - m_lineStart;
            } else {
                // Look for a natural linebreak boundary in the column
                // (We look from the end, so that the first found boundary is
                // the right one)
                size_t newLineLength = maxLineLength;
                while ( newLineLength > 0 && !isBoundary( current_line, m_lineStart + newLineLength ) ) {
                    --newLineLength;
                }
                while ( newLineLength > 0 &&
                        isWhitespace( current_line[m_lineStart + newLineLength - 1] ) ) {
                    --newLineLength;
                }

                // If we found one, then that is where we linebreak
                if ( newLineLength > 0 ) {
                    m_lineLength = newLineLength;
                } else {
                    // Otherwise we have to split text with a hyphen
                    m_addHyphen = true;
                    m_lineLength = maxLineLength - 1;
                }
            }
        }

        size_t Column::const_iterator::indentSize() const {
            auto initial =
                m_lineStart == 0 ? m_column.m_initialIndent : std::string::npos;
            return initial == std::string::npos ? m_column.m_indent : initial;
        }

        std::string
        Column::const_iterator::addIndentAndSuffix( size_t position,
                                              size_t length ) const {
            std::string ret;
            const auto desired_indent = indentSize();
            ret.reserve( desired_indent + length + m_addHyphen );
            ret.append( desired_indent, ' ' );
            ret.append( m_column.m_string, position, length );
            if ( m_addHyphen ) {
                ret.push_back( '-' );
            }

            return ret;
        }

        Column::const_iterator::const_iterator( Column const& column ): m_column( column ) {
            assert( m_column.m_width > m_column.m_indent );
            assert( m_column.m_initialIndent == std::string::npos ||
                    m_column.m_width > m_column.m_initialIndent );
            calcLength();
            if ( m_lineLength == 0 ) {
                m_lineStart = m_column.m_string.size();
            }
        }

        std::string Column::const_iterator::operator*() const {
            assert( m_lineStart <= m_parsedTo );
            return addIndentAndSuffix( m_lineStart, m_lineLength );
        }

        Column::const_iterator& Column::const_iterator::operator++() {
            m_lineStart += m_lineLength;
            std::string const& current_line = m_column.m_string;
            if ( m_lineStart < current_line.size() && current_line[m_lineStart] == '\n' ) {
                m_lineStart += 1;
            } else {
                while ( m_lineStart < current_line.size() &&
                        isWhitespace( current_line[m_lineStart] ) ) {
                    ++m_lineStart;
                }
            }

            if ( m_lineStart != current_line.size() ) {
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

        Columns Column::operator+( Column const& other ) {
            Columns cols;
            cols += *this;
            cols += other;
            return cols;
        }

        Columns& Columns::operator+=( Column const& col ) {
            m_columns.push_back( col );
            return *this;
        }

        Columns Columns::operator+( Column const& col ) {
            Columns combined = *this;
            combined += col;
            return combined;
        }

    } // namespace TextFlow
} // namespace Catch
