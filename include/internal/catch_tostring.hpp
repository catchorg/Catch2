/*
 *  Created by Phil on 8/5/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TOSTRING_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TOSTRING_HPP_INCLUDED

#include "catch_common.h"
#include <sstream>

#ifdef __OBJC__
#include "catch_objc_arc.hpp"
#endif

namespace Catch {
namespace Detail {

    struct NonStreamable {
        template<typename T> NonStreamable( const T& ){}
    };
    
    // If the type does not have its own << overload for ostream then
    // this one will be used instead
    inline std::ostream& operator << ( std::ostream& ss, NonStreamable ){
        return ss << "{?}";
    }
    
    template<typename T>
    inline std::string makeString( const T& value ) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }    

    template<typename T>
    inline std::string makeString( T* p ) {
        if( !p )
            return INTERNAL_CATCH_STRINGIFY( NULL );
        std::ostringstream oss;
        oss << p;
        return oss.str();
    }    

    template<typename T>
    inline std::string makeString( const T* p ) {
        if( !p )
            return INTERNAL_CATCH_STRINGIFY( NULL );
        std::ostringstream oss;
        oss << p;
        return oss.str();
    }    

} // end namespace Detail

/// \brief converts any type to a string
///
/// The default template forwards on to ostringstream - except when an 
/// ostringstream overload does not exist - in which case it attempts to detect 
/// that and writes {?}.
/// Overload (not specialise) this template for custom typs that you don't want
/// to provide an ostream overload for.
template<typename T>
std::string toString( const T& value ) {
    return Detail::makeString( value );
}
    
// Built in overloads

inline std::string toString( const std::string& value ) {
    return "\"" + value + "\"";
}

inline std::string toString( const std::wstring& value ) {
    std::ostringstream oss;
    oss << "\"";
    for(size_t i = 0; i < value.size(); ++i )
        oss << static_cast<char>( value[i] <= 0xff ? value[i] : '?');
    oss << "\"";
    return oss.str();
}

inline std::string toString( const char* const value ) {
    return value ? Catch::toString( std::string( value ) ) : std::string( "{null string}" );
}   

inline std::string toString( char* const value ) {
    return Catch::toString( static_cast<const char*>( value ) );
}        

inline std::string toString( int value ) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

inline std::string toString( unsigned long value ) {
    std::ostringstream oss;
    if( value > 8192 )
        oss << "0x" << std::hex << value;
    else
        oss << value;
    return oss.str();
}
    
inline std::string toString( unsigned int value ) {
    return toString( static_cast<unsigned long>( value ) );
}
    
inline std::string toString( const double value ) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}    

inline std::string toString( bool value ) {
    return value ? "true" : "false";
}

inline std::string toString( char value ) {
    return value < ' '
        ? toString( (unsigned int)value )
        : Detail::makeString( value );
}

inline std::string toString( signed char value ) {
    return toString( static_cast<char>( value ) );
}

#ifdef CATCH_CONFIG_CPP11_NULLPTR
inline std::string toString( std::nullptr_t ) {
    return "nullptr";
}
#endif

#ifdef __OBJC__
    inline std::string toString( NSString const * const& nsstring ) {
        return std::string( "@\"" ) + [nsstring UTF8String] + "\"";
    }
    inline std::string toString( NSString * CATCH_ARC_STRONG const& nsstring ) {
        return std::string( "@\"" ) + [nsstring UTF8String] + "\"";
    }
    inline std::string toString( NSObject* const& nsObject ) {
        return toString( [nsObject description] );
    }
#endif

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TOSTRING_HPP_INCLUDED
