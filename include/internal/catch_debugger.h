/*
 *  Created by Phil on 3/12/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_DEBUGGER_H_INCLUDED
#define TWOBLUECUBES_CATCH_DEBUGGER_H_INCLUDED

#include "catch_platform.h"

#include <string>

namespace Catch{

    bool isDebuggerActive();
    void writeToDebugConsole( std::string const& text );
}

#ifdef CATCH_PLATFORM_MAC

    // The following code snippet based on:
    // http://cocoawithlove.com/2008/03/break-into-debugger.html
    #if defined(__ppc64__) || defined(__ppc__)
        #define CATCH_TRAP() \
                __asm__("li r0, 20\nsc\nnop\nli r0, 37\nli r4, 2\nsc\nnop\n" \
                : : : "memory","r0","r3","r4" ) /* NOLINT */
    #else
        #define CATCH_TRAP() __asm__("int $3\n" : : /* NOLINT */ )
    #endif

#elif defined(CATCH_PLATFORM_LINUX)
    // If we can use inline assembler, do it because this allows us to break
    // directly at the location of the failing check instead of breaking inside
    // raise() called from it, i.e. one stack frame below.
    #if defined(__GNUC__) && (defined(__i386) || defined(__x86_64))
        #define CATCH_TRAP() asm volatile ("int $3") /* NOLINT */
    #else // Fall back to the generic way.
        #include <signal.h>

        #define CATCH_TRAP() raise(SIGTRAP)
    #endif
#elif defined(_MSC_VER)
    #define CATCH_TRAP() __debugbreak()
#elif defined(__MINGW32__)
    extern "C" __declspec(dllimport) void __stdcall DebugBreak();
    #define CATCH_TRAP() DebugBreak()
#endif

#ifdef CATCH_TRAP
    #define CATCH_BREAK_INTO_DEBUGGER() if( Catch::isDebuggerActive() ) { CATCH_TRAP(); }
#else
    #define CATCH_BREAK_INTO_DEBUGGER() Catch::alwaysTrue();
#endif

#endif // TWOBLUECUBES_CATCH_DEBUGGER_H_INCLUDED
