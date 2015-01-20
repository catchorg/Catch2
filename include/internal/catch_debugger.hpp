/*
 *  Created by Phil on 27/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_DEBUGGER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_DEBUGGER_HPP_INCLUDED

#include "catch_debugger.h"

#include <iostream>

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
        bool isDebuggerActive(){

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
            if( sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0) != 0 ) {
                std::cerr << "\n** Call to sysctl failed - unable to determine if debugger is active **\n" << std::endl;
                return false;
            }

            // We're being debugged if the P_TRACED flag is set.

            return ( (info.kp_proc.p_flag & P_TRACED) != 0 );
        }
    } // namespace Catch

#elif defined(_MSC_VER)
    extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
    namespace Catch {
        bool isDebuggerActive() {
            return IsDebuggerPresent() != 0;
        }
    }
#elif defined(__MINGW32__)
    extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
    namespace Catch {
        bool isDebuggerActive() {
            return IsDebuggerPresent() != 0;
        }
    }
#elif defined(CATCH_PLATFORM_LINUX)

#include <errno.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

    namespace Catch {
        bool isDebuggerActive() {
            // Based on: http://stackoverflow.com/a/3599394/1281937

            pid_t child_pid = fork();

            if (child_pid == -1)
                return false;           // Don't know; be pessimistic.

            if (!child_pid) {
                pid_t parent_pid = getppid();
                
                errno = 0;
                if (ptrace(PTRACE_ATTACH, parent_pid, 0, 0) == -1) {
                    // We can't attach when there's something already there
                    // (like a debugger) -- EPERM will indicate.
                    if (errno == EPERM)
                        _Exit(1);
                    _Exit(0);
                }
                
                // A successful attach will stop the parent.
                waitpid(parent_pid, NULL, 0);
                ptrace(PTRACE_DETACH, parent_pid, 0, 0);
                
                _Exit(0);
            }

            int result;
            waitpid(child_pid, &result, 0);
            return WEXITSTATUS(result)? true: false;
        }
    }
#else
    namespace Catch {
       inline bool isDebuggerActive() { return false; }
    }
#endif // Platform

#ifdef CATCH_PLATFORM_WINDOWS
    extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA( const char* );
    namespace Catch {
        void writeToDebugConsole( std::string const& text ) {
            ::OutputDebugStringA( text.c_str() );
        }
    }
#else
    namespace Catch {
        void writeToDebugConsole( std::string const& text ) {
            // !TBD: Need a version for Mac/ XCode and other IDEs
            std::cout << text;
        }
    }
#endif // Platform

#endif // TWOBLUECUBES_CATCH_DEBUGGER_HPP_INCLUDED
