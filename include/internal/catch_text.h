/*
 *  Created by Phil on 18/4/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEXT_H_INCLUDED
#define TWOBLUECUBES_CATCH_TEXT_H_INCLUDED

#include <string>
#include <vector>

namespace Catch {
    
    struct TextAttributes {
        TextAttributes()
        :   initialIndent( std::string::npos ),
            indent( 0 ),
            width( CATCH_CONFIG_CONSOLE_WIDTH-1 ),
            tabChar( '\t' )
        {}
        
        TextAttributes& setInitialIndent( std::size_t _value )  { initialIndent = _value; return *this; }
        TextAttributes& setIndent( std::size_t _value )         { indent = _value; return *this; }
        TextAttributes& setWidth( std::size_t _value )          { width = _value; return *this; }
        TextAttributes& setTabChar( char _value )               { tabChar = _value; return *this; }

        std::size_t initialIndent;  // indent of first line, or npos
        std::size_t indent;         // indent of subsequent lines, or all if initialIndent is npos
        std::size_t width;          // maximum width of text, including indent. Longer text will wrap
        char tabChar;               // If this char is seen the indent is changed to current pos
    };

    class Text {
    public:
        Text( std::string const& _str, TextAttributes const& _attr = TextAttributes() )
        : attr( _attr )
        {
            std::string wrappableChars = " [({.,/|\\-";
            std::size_t indent = _attr.initialIndent != std::string::npos
                ? _attr.initialIndent
                : _attr.indent;
            std::string remainder = _str;
            std::size_t tabPos = std::string::npos;
            while( !remainder.empty() ) {
                std::size_t width = _attr.width - indent;
                std::size_t wrapPos = width;
                std::size_t pos = remainder.find_first_of( '\n' );
                if( pos <= width ) {
                    wrapPos = pos;
                    addLine( indent, remainder.substr( 0, pos ) );
                    remainder = remainder.substr( pos+1 );
                    if( remainder.empty() )
                        addLine (indent, "" ); // Trailing newlines result in extra line
                }
                else {
                    pos = remainder.find_last_of( _attr.tabChar, width );
                    if( pos != std::string::npos ) {
                        tabPos = pos;
                        remainder = remainder.substr( 0, tabPos ) + remainder.substr( tabPos+1 );
                    }
                    if( remainder.size() <= width ) {
                        addLine( indent, remainder );
                        remainder = std::string();
                    }
                    else {                
                        pos = remainder.find_last_of( wrappableChars, width );
                        if( pos == std::string::npos ) {
                            addLine( indent, remainder.substr( 0, width-1 ) + "-" );
                            remainder = remainder.substr( width-1 );
                        }
                        else {
                            addLine( indent, remainder.substr( 0, pos ) );
                            if( remainder[pos] == ' ' )
                                pos++;
                            remainder = remainder.substr( pos );
                        }
                    }
                }
                indent = tabPos == std::string::npos
                    ? _attr.indent
                    : indent + tabPos;
            };
        }
        void addLine( std::size_t indent, std::string const& _line ) {
            lines.push_back( std::string( indent, ' ' ) + _line );
        }
        typedef std::vector<std::string>::const_iterator const_iterator;

        const_iterator begin() const { return lines.begin(); }
        const_iterator end() const { return lines.end(); }
        std::string const& last() const { return lines.back(); }
        std::size_t size() const { return lines.size(); }
        std::string const& operator[]( std::size_t _index ) const { return lines[_index]; }

        friend std::ostream& operator << ( std::ostream& _stream, Text const& _text ) {
            for( Text::const_iterator it = _text.begin(), itEnd = _text.end();
                it != itEnd; ++it ) {
                if( it != _text.begin() )
                    _stream << "\n";
                _stream << *it;
            }
            return _stream;
        }
        std::string toString() const {
            std::ostringstream oss;
            oss << *this;
            return oss.str();
        }
        
    private:
        std::string str;
        TextAttributes attr;
        std::vector<std::string> lines;
    };
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TEXT_H_INCLUDED
