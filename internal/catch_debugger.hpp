/*
 *  catch_debugger.hpp
 *  Catch
 *
 *  Created by Phil on 27/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 * Provides a DebugBreak() macro for platforms other than Windows
 * (currently only Macs are supported)
 */

#ifndef TWOBLUECUBES_CATCH_DEBUGGER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_DEBUGGER_HPP_INCLUDED

#include <iostream>

#if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)

    #include <assert.h>
    #include <stdbool.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <sys/sysctl.h>

    namespace Catch
    {
        // The following function is taken directly from the following technical note:
        // http://developer.apple.com/library/mac/#qa/qa2004/qa1361.html
            
        inline bool AmIBeingDebugged(void)
        // Returns true if the current process is being debugged (either
        // running under the debugger or has a debugger attached post facto).
        {
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
            #define DebugBreak() \
            if( Catch::AmIBeingDebugged() ) \
            { \
            __asm__("li r0, 20\nsc\nnop\nli r0, 37\nli r4, 2\nsc\nnop\n" \
            : : : "memory","r0","r3","r4" ); \
            }
        #else
            #define DebugBreak() if( Catch::AmIBeingDebugged() ) {__asm__("int $3\n" : : );}
        #endif
    #endif

#endif

#ifndef DebugBreak
    inline void DebugBreak(){}
#endif

inline void writeToDebugConsole( const std::string& text )
{
    // !TBD: Get Windows version working
    // - need to declare OutputDebugStringA without Windows.h
//#ifdef _WIN32
//    ::OutputDebugStringA( text.c_str() );    
//#else
    // !TBD: Need a version for Mac/ XCode and other IDEs
    std::cout << text;
//#endif
}

#endif // TWOBLUECUBES_CATCH_DEBUGGER_HPP_INCLUDED