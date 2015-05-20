/*
 *  Created by Phil on 23/4/2014.
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TOSTRING_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TOSTRING_HPP_INCLUDED

#include "catch_tostring.h"
#include "catch_interfaces_config.h"

namespace Catch {

namespace Detail {

    std::string unprintableString = "{?}";

    namespace {
        struct Endianness {
            enum Arch { Big, Little };

            static Arch which() {
                union _{
                    int asInt;
                    char asChar[sizeof (int)];
                } u;

                u.asInt = 1;
                return ( u.asChar[sizeof(int)-1] == 1 ) ? Big : Little;
            }
        };
    }

    std::string rawMemoryToString( const void *object, std::size_t size )
    {
        // Reverse order for little endian architectures
        int i = 0, end = static_cast<int>( size ), inc = 1;
        if( Endianness::which() == Endianness::Little ) {
            i = end-1;
            end = inc = -1;
        }

        unsigned char const *bytes = static_cast<unsigned char const *>(object);
        std::ostringstream os;
        os << "0x" << std::setfill('0') << std::hex;
        for( ; i != end; i += inc )
             os << std::setw(2) << static_cast<unsigned>(bytes[i]);
       return os.str();
    }
}

std::string toString( std::string const& value ) {
    std::string s = value;
    if( getCurrentContext().getConfig()->showInvisibles() ) {
        for(size_t i = 0; i < s.size(); ++i ) {
            std::string subs;
            switch( s[i] ) {
            case '\n': subs = "\\n"; break;
            case '\t': subs = "\\t"; break;
            default: break;
            }
            if( !subs.empty() ) {
                s = s.substr( 0, i ) + subs + s.substr( i+1 );
                ++i;
            }
        }
    }
    return "\"" + s + "\"";
}
std::string toString( std::wstring const& value ) {

    std::string s;
    s.reserve( value.size() );
    for(size_t i = 0; i < value.size(); ++i )
        s += value[i] <= 0xff ? static_cast<char>( value[i] ) : '?';
    return Catch::toString( s );
}

std::string toString( const char* const value ) {
    return value ? Catch::toString( std::string( value ) ) : std::string( "{null string}" );
}

std::string toString( char* const value ) {
    return Catch::toString( static_cast<const char*>( value ) );
}

std::string toString( const wchar_t* const value )
{
	return value ? Catch::toString( std::wstring(value) ) : std::string( "{null string}" );
}

std::string toString( wchar_t* const value )
{
	return Catch::toString( static_cast<const wchar_t*>( value ) );
}

std::string toString( int value ) {
    std::ostringstream oss;
    oss << value;
    if( value >= 255 )
        oss << " (0x" << std::hex << value << ")";
    return oss.str();
}

std::string toString( unsigned long value ) {
    std::ostringstream oss;
    oss << value;
    if( value >= 255 )
        oss << " (0x" << std::hex << value << ")";
    return oss.str();
}

std::string toString( unsigned int value ) {
    return Catch::toString( static_cast<unsigned long>( value ) );
}

template<typename T>
std::string fpToString( T value, int precision ) {
    std::ostringstream oss;
    oss << std::setprecision( precision )
        << std::fixed
        << value;
    std::string d = oss.str();
    std::size_t i = d.find_last_not_of( '0' );
    if( i != std::string::npos && i != d.size()-1 ) {
        if( d[i] == '.' )
            i++;
        d = d.substr( 0, i+1 );
    }
    return d;
}

std::string toString( const double value ) {
    return fpToString( value, 10 );
}
std::string toString( const float value ) {
    return fpToString( value, 5 ) + "f";
}

std::string toString( bool value ) {
    return value ? "true" : "false";
}

std::string toString( char value ) {
    return value < ' '
        ? toString( static_cast<unsigned int>( value ) )
        : Detail::makeString( value );
}

std::string toString( signed char value ) {
    return toString( static_cast<char>( value ) );
}

std::string toString( unsigned char value ) {
    return toString( static_cast<char>( value ) );
}

#ifdef CATCH_CONFIG_CPP11_NULLPTR
std::string toString( std::nullptr_t ) {
    return "nullptr";
}
#endif

#ifdef __OBJC__
    std::string toString( NSString const * const& nsstring ) {
        if( !nsstring )
            return "nil";
        return "@" + toString([nsstring UTF8String]);
    }
    std::string toString( NSString * CATCH_ARC_STRONG const& nsstring ) {
        if( !nsstring )
            return "nil";
        return "@" + toString([nsstring UTF8String]);
    }
    std::string toString( NSObject* const& nsObject ) {
        return toString( [nsObject description] );
    }
#endif

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TOSTRING_HPP_INCLUDED
