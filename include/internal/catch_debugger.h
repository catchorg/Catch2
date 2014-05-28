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
    #ifdef DEBUG
        #if defined(__ppc64__) || defined(__ppc__)
            #define CATCH_BREAK_INTO_DEBUGGER() \
                if( Catch::isDebuggerActive() ) { \
                    __asm__("li r0, 20\nsc\nnop\nli r0, 37\nli r4, 2\nsc\nnop\n" \
                    : : : "memory","r0","r3","r4" ); \
                }
        #else
            #define CATCH_BREAK_INTO_DEBUGGER() if( Catch::isDebuggerActive() ) {__asm__("int $3\n" : : );}
        #endif
    #endif

#elif defined(_MSC_VER)
    #define CATCH_BREAK_INTO_DEBUGGER() if( Catch::isDebuggerActive() ) { __debugbreak(); }
#elif defined(__MINGW32__)
    extern "C" __declspec(dllimport) void __stdcall DebugBreak();
    #define CATCH_BREAK_INTO_DEBUGGER() if( Catch::isDebuggerActive() ) { DebugBreak(); }
#endif

#ifndef CATCH_BREAK_INTO_DEBUGGER
#define CATCH_BREAK_INTO_DEBUGGER() Catch::alwaysTrue();
#endif

#endif // TWOBLUECUBES_CATCH_DEBUGGER_H_INCLUDED
