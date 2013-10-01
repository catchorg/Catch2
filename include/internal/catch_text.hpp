/*
 *  Created by Phil on 20/4/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEXT_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TEXT_HPP_INCLUDED

#include <string>
#include <vector>

namespace Catch {

    Text::Text( std::string const& _str, TextAttributes const& _attr )
    : attr( _attr )
    {
        std::string wrappableChars = " [({.,/|\\-";
        std::size_t indent = _attr.initialIndent != std::string::npos
            ? _attr.initialIndent
            : _attr.indent;
        std::string remainder = _str;

        while( !remainder.empty() ) {
            assert( lines.size() < 1000 );
            std::size_t tabPos = std::string::npos;
            std::size_t width = (std::min)( remainder.size(), _attr.width - indent );
            std::size_t pos = remainder.find_first_of( '\n' );
            if( pos <= width ) {
                width = pos;
            }
            pos = remainder.find_last_of( _attr.tabChar, width );
            if( pos != std::string::npos ) {
                tabPos = pos;
                if( remainder[width] == '\n' )
                    width--;
                remainder = remainder.substr( 0, tabPos ) + remainder.substr( tabPos+1 );
            }

            if( width == remainder.size() ) {
                spliceLine( indent, remainder, width );
            }
            else if( remainder[width] == '\n' ) {
                spliceLine( indent, remainder, width );
                if( width <= 1 || remainder.size() != 1 )
                    remainder = remainder.substr( 1 );
                indent = _attr.indent;
            }
            else {
                pos = remainder.find_last_of( wrappableChars, width );
                if( pos != std::string::npos && pos > 0 ) {
                    spliceLine( indent, remainder, pos );
                    if( remainder[0] == ' ' )
                        remainder = remainder.substr( 1 );
                }
                else {
                    spliceLine( indent, remainder, width-1 );
                    lines.back() += "-";
                }
                if( lines.size() == 1 )
                    indent = _attr.indent;
                if( tabPos != std::string::npos )
                    indent += tabPos;
            }
        }
    }

    void Text::spliceLine( std::size_t _indent, std::string& _remainder, std::size_t _pos ) {
        lines.push_back( std::string( _indent, ' ' ) + _remainder.substr( 0, _pos ) );
        _remainder = _remainder.substr( _pos );
    }

    std::string Text::toString() const {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }

    std::ostream& operator << ( std::ostream& _stream, Text const& _text ) {
        for( Text::const_iterator it = _text.begin(), itEnd = _text.end();
            it != itEnd; ++it ) {
            if( it != _text.begin() )
                _stream << "\n";
            _stream << *it;
        }
        return _stream;
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TEXT_HPP_INCLUDED
