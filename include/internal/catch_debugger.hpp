/*
 *  Created by Phil on 27/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 * Provides a BreakIntoDebugger() macro for Windows and Mac (so far)
 */
#ifndef TWOBLUECUBES_CATCH_DEBUGGER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_DEBUGGER_HPP_INCLUDED

#include <iostream>

#if defined(__MAC_OS_X_VERSION_MIN_REQUIRED)
#define CATCH_PLATFORM_MAC
#elif  defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
#define CATCH_PLATFORM_IPHONE
#elif defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER)
#define CATCH_PLATFORM_WINDOWS
#endif

#ifdef CATCH_PLATFORM_MAC

    #include <assert.h>
    #include <stdbool.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <sys/sysctl.h>

    namespace Catch{

        // The following function is taken directly from the following technical note:
        // http://developer.apple.com/library/mac/#qa/qa2004/qa1361.html

        // Returns true if the current process is being debugged (either
        // running under the debugger or has a debugger attached post facto).
        inline bool isDebuggerActive(){

            int                 junk;
            int                 mib[4];
            struct kinfo_proc   info;
            size_t              size;

            // Initialize the flags so that, if sysctl fails for some bizarre
            // reason, we get a predictable result.

            info.kp_proc.p_flag = 0;

            // Initialize mib, which tells sysctl the info we want, in this case
            // we're looking for information about a specific process ID.

            mib[0] = CTL_KERN;
            mib[1] = KERN_PROC;
            mib[2] = KERN_PROC_PID;
            mib[3] = getpid();

            // Call sysctl.

            size = sizeof(info);
            junk = sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);
            assert(junk == 0);

            // We're being debugged if the P_TRACED flag is set.

            return ( (info.kp_proc.p_flag & P_TRACED) != 0 );
        }
    }

    // The following code snippet taken from:
    // http://cocoawithlove.com/2008/03/break-into-debugger.html
    #ifdef DEBUG
        #if defined(__ppc64__) || defined(__ppc__)
            #define BreakIntoDebugger() \
            if( Catch::isDebuggerActive() ) { \
            __asm__("li r0, 20\nsc\nnop\nli r0, 37\nli r4, 2\nsc\nnop\n" \
            : : : "memory","r0","r3","r4" ); \
            }
        #else
            #define BreakIntoDebugger() if( Catch::isDebuggerActive() ) {__asm__("int $3\n" : : );}
        #endif
    #else
        inline void BreakIntoDebugger(){}
    #endif

#elif defined(_MSC_VER)
    extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
    #define BreakIntoDebugger() if (IsDebuggerPresent() ) { __debugbreak(); }
    inline bool isDebuggerActive() {
        return IsDebuggerPresent() != 0;
    }
#elif defined(__MINGW32__)
    extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
    extern "C" __declspec(dllimport) void __stdcall DebugBreak();
    #define BreakIntoDebugger() if (IsDebuggerPresent() ) { DebugBreak(); }
    inline bool isDebuggerActive() {
        return IsDebuggerPresent() != 0;
    }
#else
       inline void BreakIntoDebugger(){}
       inline bool isDebuggerActive() { return false; }
#endif

#ifdef CATCH_PLATFORM_WINDOWS
extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA( const char* );
inline void writeToDebugConsole( std::string const& text ) {
    ::OutputDebugStringA( text.c_str() );
}
#else
inline void writeToDebugConsole( std::string const& text ) {
    // !TBD: Need a version for Mac/ XCode and other IDEs
    std::cout << text;
}
#endif // CATCH_PLATFORM_WINDOWS

#endif // TWOBLUECUBES_CATCH_DEBUGGER_HPP_INCLUDED
