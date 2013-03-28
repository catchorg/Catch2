/*
 *  Created by Phil on 11/1/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_LINE_WRAP_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_LINE_WRAP_HPP_INCLUDED

#include "catch_line_wrap.h"

namespace Catch {
    
    LineWrapper::LineWrapper()
    :   right( CATCH_CONFIG_CONSOLE_WIDTH-1 ),
        nextTab( 0 ),
        tab( 0 ),
        wrappableChars( " [({.," ),
        recursionCount( 0 )
    {}
    
    LineWrapper& LineWrapper::setIndent( std::size_t _indent ) {
        indent = std::string( _indent, ' ' );
        return *this;
    }
    LineWrapper& LineWrapper::setRight( std::size_t _right ) {
        right = _right;
        return *this;
    }
    LineWrapper& LineWrapper::wrap( std::string const& _str ) {
        nextTab = tab = 0;
        wrapInternal( _str );
        return *this;
    }
    bool LineWrapper::isWrapPoint( char c ) {
        return wrappableChars.find( c ) != std::string::npos;
    }
    void LineWrapper::wrapInternal( std::string const& _str ) {
        assert( ++recursionCount < 100 );

        std::size_t width = right - indent.size();
        std::size_t wrapPoint = width-tab;
        for( std::size_t pos = 0; pos < _str.size(); ++pos ) {
            if( _str[pos] == '\n' )
            {
                addLine( _str.substr( 0, pos ) );
                nextTab = tab = 0;
                return wrapInternal( _str.substr( pos+1 ) );
            }                
            if( pos == width-tab ) {
                if( _str[wrapPoint] == ' ' ) {
                    addLine( _str.substr( 0, wrapPoint ) );
                    while( _str[++wrapPoint] == ' ' );
                }
                else if( isWrapPoint( _str[wrapPoint] ) ) {
                    addLine( _str.substr( 0, wrapPoint ) );
                }
                else {
                    addLine( _str.substr( 0, --wrapPoint ) + '-' );
                }
                return wrapInternal( _str.substr( wrapPoint ) );
            }
            if( _str[pos] == '\t' ) {
                nextTab = pos;
                std::string withoutTab = _str.substr( 0, nextTab ) + _str.substr( nextTab+1 );
                return wrapInternal( withoutTab );
            }
            else if( isWrapPoint( _str[pos] ) ) {
                wrapPoint = pos;
            }
        }
        addLine( _str );
    }

    std::ostream& operator << ( std::ostream& _stream, LineWrapper const& _lineWrapper ) {
        for( LineWrapper::const_iterator it = _lineWrapper.begin(), itEnd = _lineWrapper.end();
            it != itEnd; ++it ) {
            if( it != _lineWrapper.begin() )
                _stream << "\n";
            _stream << *it;
        }
        return _stream;
    }
    std::string LineWrapper::toString() const {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }
    
    void LineWrapper::addLine( const std::string& _line ) {
        if( tab > 0 )
            lines.push_back( indent + std::string( tab, ' ' ) + _line );
        else
            lines.push_back( indent + _line );
        if( nextTab > 0 )
            tab = nextTab;
    }
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_LINE_WRAP_HPP_INCLUDED
