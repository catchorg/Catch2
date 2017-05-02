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

    const std::string unprintableString = "{?}";

    namespace {
        const int hexThreshold = 255;

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

    std::string rawMemoryToString( const void *object, std::size_t size ) {
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


//// ======================================================= ////
//
//   Out-of-line defs for full specialization of StringMaker
//
//// ======================================================= ////

std::string StringMaker<std::string>::operator()(const std::string& str) {
    if (!getCurrentContext().getConfig()->showInvisibles()) {
        return '"' + str + '"';
    }

    std::string s("\"");
    for (char c : str) {
        switch (c) {
        case '\n':
            s.append("\\n");
            break;
        case '\t':
            s.append("\\t");
            break;
        default:
            s.push_back(c);
            break;
        }
    }
    s.append("\"");
    return s;
}

std::string StringMaker<std::wstring>::operator()(const std::wstring& wstr) {
    std::string s;
    s.reserve(wstr.size());
    for (auto c : wstr) {
        s += (c <= 0xff) ? static_cast<char>(c) : '?';
    }
    return ::Catch::Detail::stringify(s);
}

std::string StringMaker<char const*>::operator()(char const* str) {
    if (str) {
        return ::Catch::Detail::stringify(std::string{ str });
    } else {
        return{ "{null string}" };
    }
}
std::string StringMaker<char*>::operator()(char* str) {
    if (str) {
        return ::Catch::Detail::stringify(std::string{ str });
    } else {
        return{ "{null string}" };
    }
}
std::string StringMaker<wchar_t const*>::operator()(wchar_t const * str) {
    if (str) {
        return ::Catch::Detail::stringify(std::wstring{ str });
    } else {
        return{ "{null string}" };
    }
}
std::string StringMaker<wchar_t *>::operator()(wchar_t * str) {
    if (str) {
        return ::Catch::Detail::stringify(std::wstring{ str });
    } else {
        return{ "{null string}" };
    }
}


std::string StringMaker<int>::operator()(int value) {
    return ::Catch::Detail::stringify(static_cast<long long>(value));
}
std::string StringMaker<long>::operator()(long value) {
    return ::Catch::Detail::stringify(static_cast<long long>(value));
}
std::string StringMaker<long long>::operator()(long long value) {
    std::ostringstream oss;
    oss << value;
    if (value > Detail::hexThreshold) {
        oss << " (0x" << std::hex << value << ')';
    }
    return oss.str();
}

std::string StringMaker<unsigned int>::operator()(unsigned int value) {
    return ::Catch::Detail::stringify(static_cast<unsigned long long>(value));
}
std::string StringMaker<unsigned long>::operator()(unsigned long value) {
    return ::Catch::Detail::stringify(static_cast<unsigned long long>(value));
}
std::string StringMaker<unsigned long long>::operator()(unsigned long long value) {
    std::ostringstream oss;
    oss << value;
    if (value > Detail::hexThreshold) {
        oss << " (0x" << std::hex << value << ')';
    }
    return oss.str();
}


std::string StringMaker<bool>::operator()(bool b) {
    return b ? "true" : "false";
}

std::string StringMaker<char>::operator()(char value) {
    if (value == '\r') {
        return "'\\r'";
    } else if (value == '\f') {
        return "'\\f'";
    } else if (value == '\n') {
        return "'\\n'";
    } else if (value == '\t') {
        return "'\\t'";
    } else if ('\0' <= value && value < ' ') {
        return ::Catch::Detail::stringify(static_cast<unsigned int>(value));
    } else {
        char chstr[] = "' '";
        chstr[1] = value;
        return chstr;
    }
}
std::string StringMaker<signed char>::operator()(signed char c) {
    return ::Catch::Detail::stringify(static_cast<char>(c));
}
std::string StringMaker<unsigned char>::operator()(unsigned char c) {
    return ::Catch::Detail::stringify(static_cast<char>(c));
}

std::string StringMaker<std::nullptr_t>::operator()(std::nullptr_t) {
    return "nullptr";
}

std::string StringMaker<float>::operator()(float value) {
    return fpToString(value, 5) + 'f';
}
std::string StringMaker<double>::operator()(double value) {
    return fpToString(value, 10);
}


#ifdef __OBJC__
std::string StringMaker<NSString const *>::operator()(NSString const * const& nsstring) {
    if (!nsstring)
        return "nil";
    return "@" + toString([nsstring UTF8String]);
}
std::string StringMaker<NSString * CATCH_ARC_STRONG>::operator()(NSString * CATCH_ARC_STRONG const& nsstring) {
    if (!nsstring)
        return "nil";
    return "@" + toString([nsstring UTF8String]);
}
std::string StringMaker<NSObject *>::operator()(NSObject * const& nsObject) {
    return ::Catch::Detail::stringify([nsObject description]);
}
#endif

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TOSTRING_HPP_INCLUDED
