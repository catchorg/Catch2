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
    
    namespace {
        inline void addIndent( std::ostream& os, std::size_t indent ) {
            while( indent-- > 0 )
                os << ' ';
        }
        
        inline void recursivelyWrapLine( std::ostream& os, std::string paragraph, std::size_t columns, std::size_t indent ) {
            std::size_t width = columns-indent;
            std::size_t tab = 0;
            std::size_t wrapPoint = width;
            for( std::size_t pos = 0; pos < paragraph.size(); ++pos ) {
                if( pos == width ) {
                    addIndent( os, indent );
                    os << paragraph.substr( 0, wrapPoint ) << "\n";
                    return recursivelyWrapLine( os, paragraph.substr( wrapPoint+1 ), columns, indent+tab );
                }
                if( paragraph[pos] == '\t' ) {
                    tab = pos;
                    paragraph = paragraph.substr( 0, tab ) + paragraph.substr( tab+1 );
                    pos--;
                }
                else if( paragraph[pos] == ' ' ) {
                    wrapPoint = pos;
                }
            }
            addIndent( os, indent );
            os << paragraph;
        }
    }

    void wrapLongStrings( std::ostream& stream, const std::string& str, std::size_t columns, std::size_t indent ) {
        std::string::size_type pos = 0;
        std::string::size_type newline = str.find_first_of( '\n' );
        while( newline != std::string::npos ) {
            std::string paragraph = str.substr( pos, newline-pos );
            recursivelyWrapLine( stream, paragraph, columns, indent );
            stream << "\n";
            pos = newline+1;
            newline = str.find_first_of( '\n', pos );
        }
        if( pos != str.size() )
            recursivelyWrapLine( stream, str.substr( pos, str.size()-pos ), columns, indent );
    }

    std::string wrapLongStrings( const std::string& str, std::size_t columns, std::size_t indent ) {
        std::ostringstream oss;
        wrapLongStrings( oss, str, columns, indent );
        return oss.str();        
    }
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_LINE_WRAP_HPP_INCLUDED
