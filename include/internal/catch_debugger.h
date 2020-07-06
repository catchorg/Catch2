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

namespace Catch {
    bool isDebuggerActive();
}

#ifdef CATCH_PLATFORM_MAC

    #if defined(__i386__) || defined(__x86_64__)
        #define CATCH_TRAP() __asm__("int $3\n" : : ) /* NOLINT */
    #elif defined(__aarch64__)
        #define CATCH_TRAP()  __asm__(".inst 0xd4200000")
    #endif

#elif defined(CATCH_PLATFORM_IPHONE)

    // use inline assembler
    #if defined(__i386__) || defined(__x86_64__)
        #define CATCH_TRAP()  __asm__("int $3")
    #elif defined(__aarch64__)
        #define CATCH_TRAP()  __asm__(".inst 0xd4200000")
    #elif defined(__arm__) && !defined(__thumb__)
        #define CATCH_TRAP()  __asm__(".inst 0xe7f001f0")
    #elif defined(__arm__) &&  defined(__thumb__)
        #define CATCH_TRAP()  __asm__(".inst 0xde01")
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

#ifndef CATCH_BREAK_INTO_DEBUGGER
    #ifdef CATCH_TRAP
        #define CATCH_BREAK_INTO_DEBUGGER() []{ if( Catch::isDebuggerActive() ) { CATCH_TRAP(); } }()
    #else
        #define CATCH_BREAK_INTO_DEBUGGER() []{}()
    #endif
#endif

#endif // TWOBLUECUBES_CATCH_DEBUGGER_H_INCLUDED
