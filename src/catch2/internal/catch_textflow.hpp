
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_TEXTFLOW_HPP_INCLUDED
#define CATCH_TEXTFLOW_HPP_INCLUDED

#include <cassert>
#include <catch2/internal/catch_console_width.hpp>
#include <string>
#include <vector>

namespace Catch {
    namespace TextFlow {

        class Columns;

        /**
         * Represents a column of text with specific width and indentation
         *
         * When written out to a stream, it will perform linebreaking
         * of the provided text so that the written lines fit within
         * target width.
         */
        class Column {
            // String to be written out
            std::string m_string;
            // Width of the column for linebreaking
            size_t m_width = CATCH_CONFIG_CONSOLE_WIDTH - 1;
            // Indentation of other lines (including first if initial indent is unset)
            size_t m_indent = 0;
            // Indentation of the first line
            size_t m_initialIndent = std::string::npos;

        public:
            /**
             * Iterates "lines" in `Column` and return sthem
             */
            class const_iterator {
                friend Column;
                struct EndTag {};

                Column const& m_column;
                // Where does the current line start?
                size_t m_lineStart = 0;
                // How long should the current line be?
                size_t m_lineLength = 0;
                // How far have we checked the string to iterate?
                size_t m_parsedTo = 0;
                // Should a '-' be appended to the line?
                bool m_addHyphen = false;

                const_iterator( Column const& column, EndTag ):
                    m_column( column ), m_lineStart( m_column.m_string.size() ) {}

                // Calculates the length of the current line
                void calcLength();

                // Returns current indention width
                size_t indentSize() const;

                // Creates an indented and (optionally) suffixed string from
                // current iterator position, indentation and length.
                std::string addIndentAndSuffix( size_t position,
                                                size_t length ) const;

            public:
                using difference_type = std::ptrdiff_t;
                using value_type = std::string;
                using pointer = value_type*;
                using reference = value_type&;
                using iterator_category = std::forward_iterator_tag;

                explicit const_iterator( Column const& column );

                std::string operator*() const;

                const_iterator& operator++();
                const_iterator operator++( int );

                bool operator==( const_iterator const& other ) const {
                    return m_lineStart == other.m_lineStart && &m_column == &other.m_column;
                }
                bool operator!=( const_iterator const& other ) const {
                    return !operator==( other );
                }
            };
            using iterator = const_iterator;

            explicit Column( std::string const& text ): m_string( text ) {}

            Column& width( size_t newWidth ) {
                assert( newWidth > 0 );
                m_width = newWidth;
                return *this;
            }
            Column& indent( size_t newIndent ) {
                m_indent = newIndent;
                return *this;
            }
            Column& initialIndent( size_t newIndent ) {
                m_initialIndent = newIndent;
                return *this;
            }

            size_t width() const { return m_width; }
            const_iterator begin() const { return const_iterator( *this ); }
            const_iterator end() const { return { *this, const_iterator::EndTag{} }; }

            friend std::ostream& operator<<( std::ostream& os,
                                             Column const& col );

            Columns operator+( Column const& other );
        };

        //! Creates a column that serves as an empty space of specific width
        Column Spacer( size_t spaceWidth );

        class Columns {
            std::vector<Column> m_columns;

        public:
            class iterator {
                friend Columns;
                struct EndTag {};

                std::vector<Column> const& m_columns;
                std::vector<Column::const_iterator> m_iterators;
                size_t m_activeIterators;

                iterator( Columns const& columns, EndTag );

            public:
                using difference_type = std::ptrdiff_t;
                using value_type = std::string;
                using pointer = value_type*;
                using reference = value_type&;
                using iterator_category = std::forward_iterator_tag;

                explicit iterator( Columns const& columns );

                auto operator==( iterator const& other ) const -> bool {
                    return m_iterators == other.m_iterators;
                }
                auto operator!=( iterator const& other ) const -> bool {
                    return m_iterators != other.m_iterators;
                }
                std::string operator*() const;
                iterator& operator++();
                iterator operator++( int );
            };
            using const_iterator = iterator;

            iterator begin() const { return iterator( *this ); }
            iterator end() const { return { *this, iterator::EndTag() }; }

            Columns& operator+=( Column const& col );
            Columns operator+( Column const& col );

            friend std::ostream& operator<<( std::ostream& os,
                                             Columns const& cols );
        };

    } // namespace TextFlow
} // namespace Catch
#endif // CATCH_TEXTFLOW_HPP_INCLUDED
