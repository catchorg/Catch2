/*
 *  Created by Phil on 27/11/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_COMMON_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_COMMON_HPP_INCLUDED

#include "catch_common.h"

namespace Catch {

    INTERNAL_CATCH_INLINE bool startsWith( std::string const& s, std::string const& prefix ) {
        return s.size() >= prefix.size() && s.substr( 0, prefix.size() ) == prefix;
    }
    INTERNAL_CATCH_INLINE bool endsWith( std::string const& s, std::string const& suffix ) {
        return s.size() >= suffix.size() && s.substr( s.size()-suffix.size(), suffix.size() ) == suffix;
    }
    INTERNAL_CATCH_INLINE bool contains( std::string const& s, std::string const& infix ) {
        return s.find( infix ) != std::string::npos;
    }
    INTERNAL_CATCH_INLINE void toLowerInPlace( std::string& s ) {
        std::transform( s.begin(), s.end(), s.begin(), ::tolower );
    }
    INTERNAL_CATCH_INLINE std::string toLower( std::string const& s ) {
        std::string lc = s;
        toLowerInPlace( lc );
        return lc;
    }
    INTERNAL_CATCH_INLINE std::string trim( std::string const& str ) {
        static char const* whitespaceChars = "\n\r\t ";
        std::string::size_type start = str.find_first_not_of( whitespaceChars );
        std::string::size_type end = str.find_last_not_of( whitespaceChars );

        return start != std::string::npos ? str.substr( start, 1+end-start ) : "";
    }

    INTERNAL_CATCH_INLINE pluralise::pluralise( std::size_t count, std::string const& label )
    :   m_count( count ),
        m_label( label )
    {}

    INTERNAL_CATCH_INLINE std::ostream& operator << ( std::ostream& os, pluralise const& pluraliser ) {
        os << pluraliser.m_count << " " << pluraliser.m_label;
        if( pluraliser.m_count != 1 )
            os << "s";
        return os;
    }

    INTERNAL_CATCH_INLINE SourceLineInfo::SourceLineInfo() : line( 0 ){}
    INTERNAL_CATCH_INLINE SourceLineInfo::SourceLineInfo( char const* _file, std::size_t _line )
    :   file( _file ),
        line( _line )
    {}
    INTERNAL_CATCH_INLINE SourceLineInfo::SourceLineInfo( SourceLineInfo const& other )
    :   file( other.file ),
        line( other.line )
    {}
    INTERNAL_CATCH_INLINE bool SourceLineInfo::empty() const {
        return file.empty();
    }
    INTERNAL_CATCH_INLINE bool SourceLineInfo::operator == ( SourceLineInfo const& other ) const {
        return line == other.line && file == other.file;
    }

    INTERNAL_CATCH_INLINE std::ostream& operator << ( std::ostream& os, SourceLineInfo const& info ) {
#ifndef __GNUG__
        os << info.file << "(" << info.line << ")";
#else
        os << info.file << ":" << info.line;
#endif
        return os;
    }

    INTERNAL_CATCH_INLINE void throwLogicError( std::string const& message, SourceLineInfo const& locationInfo ) {
        std::ostringstream oss;
        oss << locationInfo << ": Internal Catch error: '" << message << "'";
        if( isTrue( true ))
            throw std::logic_error( oss.str() );
    }
}

#endif // TWOBLUECUBES_CATCH_COMMON_HPP_INCLUDED

