/*
 *  Created by Phil on 23/4/2014.
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wexit-time-destructors"
#    pragma clang diagnostic ignored "-Wglobal-constructors"
#endif

// Enable specific decls locally
#if !defined(CATCH_CONFIG_ENABLE_CHRONO_STRINGMAKER)
#define CATCH_CONFIG_ENABLE_CHRONO_STRINGMAKER
#endif

#include "catch_tostring.h"
#include "catch_interfaces_config.h"
#include "catch_context.h"

#include <cmath>
#include <iomanip>

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
        ReusableStringStream rss;
        rss << "0x" << std::setfill('0') << std::hex;
        for( ; i != end; i += inc )
             rss << std::setw(2) << static_cast<unsigned>(bytes[i]);
       return rss.str();
    }
}


template<typename T>
std::string fpToString( T value, int precision ) {
    if (std::isnan(value)) {
        return "nan";
    }

    ReusableStringStream rss;
    rss << std::setprecision( precision )
        << std::fixed
        << value;
    std::string d = rss.str();
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

std::string StringMaker<std::string>::convert(const std::string& str) {
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

std::string StringMaker<std::wstring>::convert(const std::wstring& wstr) {
    std::string s;
    s.reserve(wstr.size());
    for (auto c : wstr) {
        s += (c <= 0xff) ? static_cast<char>(c) : '?';
    }
    return ::Catch::Detail::stringify(s);
}

std::string StringMaker<char const*>::convert(char const* str) {
    if (str) {
        return ::Catch::Detail::stringify(std::string{ str });
    } else {
        return{ "{null string}" };
    }
}
std::string StringMaker<char*>::convert(char* str) {
    if (str) {
        return ::Catch::Detail::stringify(std::string{ str });
    } else {
        return{ "{null string}" };
    }
}
std::string StringMaker<wchar_t const*>::convert(wchar_t const * str) {
    if (str) {
        return ::Catch::Detail::stringify(std::wstring{ str });
    } else {
        return{ "{null string}" };
    }
}
std::string StringMaker<wchar_t *>::convert(wchar_t * str) {
    if (str) {
        return ::Catch::Detail::stringify(std::wstring{ str });
    } else {
        return{ "{null string}" };
    }
}


std::string StringMaker<int>::convert(int value) {
    return ::Catch::Detail::stringify(static_cast<long long>(value));
}
std::string StringMaker<long>::convert(long value) {
    return ::Catch::Detail::stringify(static_cast<long long>(value));
}
std::string StringMaker<long long>::convert(long long value) {
    ReusableStringStream rss;
    rss << value;
    if (value > Detail::hexThreshold) {
        rss << " (0x" << std::hex << value << ')';
    }
    return rss.str();
}

std::string StringMaker<unsigned int>::convert(unsigned int value) {
    return ::Catch::Detail::stringify(static_cast<unsigned long long>(value));
}
std::string StringMaker<unsigned long>::convert(unsigned long value) {
    return ::Catch::Detail::stringify(static_cast<unsigned long long>(value));
}
std::string StringMaker<unsigned long long>::convert(unsigned long long value) {
    ReusableStringStream rss;
    rss << value;
    if (value > Detail::hexThreshold) {
        rss << " (0x" << std::hex << value << ')';
    }
    return rss.str();
}


std::string StringMaker<bool>::convert(bool b) {
    return b ? "true" : "false";
}

std::string StringMaker<char>::convert(char value) {
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
std::string StringMaker<signed char>::convert(signed char c) {
    return ::Catch::Detail::stringify(static_cast<char>(c));
}
std::string StringMaker<unsigned char>::convert(unsigned char c) {
    return ::Catch::Detail::stringify(static_cast<char>(c));
}

std::string StringMaker<std::nullptr_t>::convert(std::nullptr_t) {
    return "nullptr";
}

std::string StringMaker<float>::convert(float value) {
    return fpToString(value, 5) + 'f';
}
std::string StringMaker<double>::convert(double value) {
    return fpToString(value, 10);
}

std::string ratio_string<std::atto>::symbol() { return "a"; }
std::string ratio_string<std::femto>::symbol() { return "f"; }
std::string  ratio_string<std::pico>::symbol() { return "p"; }
std::string  ratio_string<std::nano>::symbol() { return "n"; }
std::string ratio_string<std::micro>::symbol() { return "u"; }
std::string ratio_string<std::milli>::symbol() { return "m"; }


} // end namespace Catch

#if defined(__clang__)
#    pragma clang diagnostic pop
#endif

