/*
 *  Created by Phil on 18/4/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
// Only use header guard if we are not using an outer namespace
#ifndef CLICHE_TBC_TEXT_FORMAT_OUTER_NAMESPACE
# ifdef TWOBLUECUBES_TEXT_FORMAT_H_INCLUDED
#  ifndef TWOBLUECUBES_TEXT_FORMAT_H_ALREADY_INCLUDED
#   define TWOBLUECUBES_TEXT_FORMAT_H_ALREADY_INCLUDED
#  endif
# else
#  define TWOBLUECUBES_TEXT_FORMAT_H_INCLUDED
# endif
#endif
#ifndef TWOBLUECUBES_TEXT_FORMAT_H_ALREADY_INCLUDED
#include <string>
#include <vector>
#include <sstream>

// Use optional outer namespace
#ifdef CLICHE_TBC_TEXT_FORMAT_OUTER_NAMESPACE
namespace CLICHE_TBC_TEXT_FORMAT_OUTER_NAMESPACE {
#endif

namespace Tbc {

#ifdef TBC_TEXT_FORMAT_CONSOLE_WIDTH
    const unsigned int consoleWidth = TBC_TEXT_FORMAT_CONSOLE_WIDTH;
#else
    const unsigned int consoleWidth = 80;
#endif

    struct TextAttributes {
        TextAttributes()
        :   initialIndent( std::string::npos ),
            indent( 0 ),
            width( consoleWidth-1 )
        {}

        TextAttributes& setInitialIndent( std::size_t _value )  { initialIndent = _value; return *this; }
        TextAttributes& setIndent( std::size_t _value )         { indent = _value; return *this; }
        TextAttributes& setWidth( std::size_t _value )          { width = _value; return *this; }

        std::size_t initialIndent;  // indent of first line, or npos
        std::size_t indent;         // indent of subsequent lines, or all if initialIndent is npos
        std::size_t width;          // maximum width of text, including indent. Longer text will wrap
    };

    class Text {
    public:
        Text( std::string const& _str, TextAttributes const& _attr = TextAttributes() )
        : attr( _attr )
        {
            const std::string wrappableBeforeChars = "[({<\t";
            const std::string wrappableAfterChars = "])}>-,./|\\";
            const std::string wrappableInsteadOfChars = " \n\r";
            std::string indent = _attr.initialIndent != std::string::npos
                ? std::string( _attr.initialIndent, ' ' )
                : std::string( _attr.indent, ' ' );

            typedef std::string::const_iterator iterator;
            iterator it = _str.begin();
            const iterator strEnd = _str.end();

            while( it != strEnd ) {

                if( lines.size() >= 1000 ) {
                    lines.push_back( "... message truncated due to excessive size" );
                    return;
                }


                std::string suffix;
                std::size_t width = (std::min)( static_cast<size_t>( strEnd-it ), _attr.width-static_cast<size_t>( indent.size() ) );
                iterator itEnd = it+width;
                iterator itNext = _str.end();

                iterator itNewLine = std::find( it, itEnd, '\n' );
                if( itNewLine != itEnd )
                    itEnd = itNewLine;

                if( itEnd != strEnd  ) {
                    bool foundWrapPoint = false;
                    iterator findIt = itEnd;
                    do {
                        if( wrappableAfterChars.find( *findIt ) != std::string::npos && findIt != itEnd ) {
                            itEnd = findIt+1;
                            itNext = findIt+1;
                            foundWrapPoint = true;
                        }
                        else if( findIt > it && wrappableBeforeChars.find( *findIt ) != std::string::npos ) {
                            itEnd = findIt;
                            itNext = findIt;
                            foundWrapPoint = true;
                        }
                        else if( wrappableInsteadOfChars.find( *findIt ) != std::string::npos ) {
                            itNext = findIt+1;
                            itEnd = findIt;
                            foundWrapPoint = true;
                        }
                        if( findIt == it )
                            break;
                        else
                            --findIt;
                    }
                    while( !foundWrapPoint );

                    if( !foundWrapPoint ) {
                        // No good wrap char, so we'll break mid word and add a hyphen
                        --itEnd;
                        itNext = itEnd;
                        suffix = "-";
                    }
                    else {
                        while( itEnd > it && wrappableInsteadOfChars.find( *(itEnd-1) ) != std::string::npos )
                            --itEnd;
                    }
                }
                lines.push_back( indent + std::string( it, itEnd ) + suffix );

                if( indent.size() != _attr.indent )
                    indent = std::string( _attr.indent, ' ' );
                it = itNext;
            }
        }



        typedef std::vector<std::string>::const_iterator const_iterator;

        const_iterator begin() const { return lines.begin(); }
        const_iterator end() const { return lines.end(); }
        std::string const& last() const { return lines.back(); }
        std::size_t size() const { return lines.size(); }
        std::string const& operator[]( std::size_t _index ) const { return lines[_index]; }
        std::string toString() const {
            std::ostringstream oss;
            oss << *this;
            return oss.str();
        }

        inline friend std::ostream& operator << ( std::ostream& _stream, Text const& _text ) {
            for( Text::const_iterator it = _text.begin(), itEnd = _text.end();
                it != itEnd; ++it ) {
                if( it != _text.begin() )
                    _stream << "\n";
                _stream << *it;
            }
            return _stream;
        }


    private:
        std::string str;
        TextAttributes attr;
        std::vector<std::string> lines;
    };

} // end namespace Tbc

#ifdef CLICHE_TBC_TEXT_FORMAT_OUTER_NAMESPACE
} // end outer namespace
#endif

#endif // TWOBLUECUBES_TEXT_FORMAT_H_ALREADY_INCLUDED
