/*
 *  Created by Phil on 29/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_COMMON_H_INCLUDED
#define TWOBLUECUBES_CATCH_COMMON_H_INCLUDED

#include "catch_compiler_capabilities.h"

#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) name##line
#define INTERNAL_CATCH_UNIQUE_NAME_LINE( name, line ) INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line )
#ifdef CATCH_CONFIG_COUNTER
#  define INTERNAL_CATCH_UNIQUE_NAME( name ) INTERNAL_CATCH_UNIQUE_NAME_LINE( name, __COUNTER__ )
#else
#  define INTERNAL_CATCH_UNIQUE_NAME( name ) INTERNAL_CATCH_UNIQUE_NAME_LINE( name, __LINE__ )
#endif

#include <iosfwd>
#include <string>
#include <cstdint>

namespace Catch {

    struct CaseSensitive { enum Choice {
        Yes,
        No
    }; };

    class NonCopyable {
        NonCopyable( NonCopyable const& )              = delete;
        NonCopyable( NonCopyable && )                  = delete;
        NonCopyable& operator = ( NonCopyable const& ) = delete;
        NonCopyable& operator = ( NonCopyable && )     = delete;

    protected:
        NonCopyable();
        virtual ~NonCopyable();
    };

    struct SourceLineInfo {

        SourceLineInfo() = delete;
        SourceLineInfo( char const* _file, std::size_t _line ) noexcept
        :   file( _file ),
            line( _line )
        {}

        SourceLineInfo( SourceLineInfo const& other )        = default;
        SourceLineInfo( SourceLineInfo && )                  = default;
        SourceLineInfo& operator = ( SourceLineInfo const& ) = default;
        SourceLineInfo& operator = ( SourceLineInfo && )     = default;

        bool empty() const noexcept;
        bool operator == ( SourceLineInfo const& other ) const noexcept;
        bool operator < ( SourceLineInfo const& other ) const noexcept;

        char const* file;
        std::size_t line;
    };

    std::ostream& operator << ( std::ostream& os, SourceLineInfo const& info );

    // Use this in variadic streaming macros to allow
    //    >> +StreamEndStop
    // as well as
    //    >> stuff +StreamEndStop
    struct StreamEndStop {
        std::string operator+() const;
    };
    template<typename T>
    T const& operator + ( T const& value, StreamEndStop ) {
        return value;
    }


    // A replacement for the "throw" keyword which is illegal when
    // exceptions are disabled with -fno-exceptions.
    struct Exception {
        template <typename E>
        [[noreturn]] static void doThrow(E&& e) {
#if CATCH_CONFIG_USE_EXCEPTIONS
            throw std::forward<E>(e);
#else
            // "throw" keyword is unsupported when exceptions are disabled.
            // Immediately terminate instead.
            (void)e;  // error: unused parameter 'e' [-Werror,-Wunused-parameter]
            std::terminate();
#endif
        }
    };
}

#define CATCH_INTERNAL_LINEINFO \
    ::Catch::SourceLineInfo( __FILE__, static_cast<std::size_t>( __LINE__ ) )

#if CATCH_CONFIG_USE_EXCEPTIONS
#  define CATCH_INTERNAL_TRY try
#  define CATCH_INTERNAL_CATCH( type, var) catch( type var )
#  define CATCH_INTERNAL_CATCH_UNNAMED( type ) catch( type )
#  define CATCH_INTERNAL_CATCH_ALL() catch( ... )
#else
// 'try' and 'catch' (and 'throw') keywords are illegal with -fno-exceptions.
#  define CATCH_INTERNAL_TRY if ((true))
#  define CATCH_INTERNAL_CATCH( typ, var ) while ((false)) for ( typename std::remove_reference<typ>::type var ; ; )
#  define CATCH_INTERNAL_CATCH_UNNAMED( type ) while ((false))
#  define CATCH_INTERNAL_CATCH_ALL() while ((false))
#endif

#endif // TWOBLUECUBES_CATCH_COMMON_H_INCLUDED

