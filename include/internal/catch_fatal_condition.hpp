/*
 *  Created by Phil on 21/08/2014
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_FATAL_CONDITION_H_INCLUDED
#define TWOBLUECUBES_CATCH_FATAL_CONDITION_H_INCLUDED


namespace Catch {

    // Report the error condition
    inline void reportFatal( std::string const& message ) {
        IContext& context = Catch::getCurrentContext();
        IResultCapture* resultCapture = context.getResultCapture();
        resultCapture->handleFatalErrorCondition( message );
    }

} // namespace Catch

#if defined ( CATCH_PLATFORM_WINDOWS ) /////////////////////////////////////////
#include "catch_windows_h_proxy.h"

#  if !defined ( CATCH_CONFIG_WINDOWS_SEH )

namespace Catch {
    struct FatalConditionHandler {
        void reset() {}
    };
}

#  else // CATCH_CONFIG_WINDOWS_SEH is defined

namespace Catch {

    struct SignalDefs { DWORD id; const char* name; };
    extern SignalDefs signalDefs[];
    // There is no 1-1 mapping between signals and windows exceptions.
    // Windows can easily distinguish between SO and SigSegV,
    // but SigInt, SigTerm, etc are handled differently.
    SignalDefs signalDefs[] = {
        { EXCEPTION_ILLEGAL_INSTRUCTION,  "SIGILL - Illegal instruction signal" },
        { EXCEPTION_STACK_OVERFLOW, "SIGSEGV - Stack overflow" },
        { EXCEPTION_ACCESS_VIOLATION, "SIGSEGV - Segmentation violation signal" },
        { EXCEPTION_INT_DIVIDE_BY_ZERO, "Divide by zero error" },
    };

    struct FatalConditionHandler {

        static LONG CALLBACK handleVectoredException(PEXCEPTION_POINTERS ExceptionInfo) {
            for (int i = 0; i < sizeof(signalDefs) / sizeof(SignalDefs); ++i) {
                if (ExceptionInfo->ExceptionRecord->ExceptionCode == signalDefs[i].id) {
                    reportFatal(signalDefs[i].name);
                }
            }
            // If its not an exception we care about, pass it along.
            // This stops us from eating debugger breaks etc.
            return EXCEPTION_CONTINUE_SEARCH;
        }

        FatalConditionHandler() {
            isSet = true;
            // 32k seems enough for Catch to handle stack overflow,
            // but the value was found experimentally, so there is no strong guarantee
            guaranteeSize = 32 * 1024;
            exceptionHandlerHandle = CATCH_NULL;
            // Register as first handler in current chain
            exceptionHandlerHandle = AddVectoredExceptionHandler(1, handleVectoredException);
            // Pass in guarantee size to be filled
            SetThreadStackGuarantee(&guaranteeSize);
        }

        static void reset() {
            if (isSet) {
                // Unregister handler and restore the old guarantee
                RemoveVectoredExceptionHandler(exceptionHandlerHandle);
                SetThreadStackGuarantee(&guaranteeSize);
                exceptionHandlerHandle = CATCH_NULL;
                isSet = false;
            }
        }

        ~FatalConditionHandler() {
            reset();
        }
    private:
        static bool isSet;
        static ULONG guaranteeSize;
        static PVOID exceptionHandlerHandle;
    };

    bool FatalConditionHandler::isSet = false;
    ULONG FatalConditionHandler::guaranteeSize = 0;
    PVOID FatalConditionHandler::exceptionHandlerHandle = CATCH_NULL;

} // namespace Catch

#  endif // CATCH_CONFIG_WINDOWS_SEH

#else // Not Windows - assumed to be POSIX compatible //////////////////////////

#  if !defined(CATCH_CONFIG_POSIX_SIGNALS)

namespace Catch {
    struct FatalConditionHandler {
        void reset() {}
    };
}


#  else // CATCH_CONFIG_POSIX_SIGNALS is defined

#include <signal.h>

namespace Catch {

    struct SignalDefs {
        int id;
        const char* name;
    };
    extern SignalDefs signalDefs[];
    SignalDefs signalDefs[] = {
            { SIGINT,  "SIGINT - Terminal interrupt signal" },
            { SIGILL,  "SIGILL - Illegal instruction signal" },
            { SIGFPE,  "SIGFPE - Floating point error signal" },
            { SIGSEGV, "SIGSEGV - Segmentation violation signal" },
            { SIGTERM, "SIGTERM - Termination request signal" },
            { SIGABRT, "SIGABRT - Abort (abnormal termination) signal" }
    };

    struct FatalConditionHandler {

        static bool isSet;
        static struct sigaction oldSigActions [sizeof(signalDefs)/sizeof(SignalDefs)];
        static stack_t oldSigStack;
        static char altStackMem[SIGSTKSZ];

        static void handleSignal( int sig ) {
            std::string name = "<unknown signal>";
            for (std::size_t i = 0; i < sizeof(signalDefs) / sizeof(SignalDefs); ++i) {
                SignalDefs &def = signalDefs[i];
                if (sig == def.id) {
                    name = def.name;
                    break;
                }
            }
            reset();
            reportFatal(name);
            raise( sig );
        }

        FatalConditionHandler() {
            isSet = true;
            stack_t sigStack;
            sigStack.ss_sp = altStackMem;
            sigStack.ss_size = SIGSTKSZ;
            sigStack.ss_flags = 0;
            sigaltstack(&sigStack, &oldSigStack);
            struct sigaction sa = { 0 };

            sa.sa_handler = handleSignal;
            sa.sa_flags = SA_ONSTACK;
            for (std::size_t i = 0; i < sizeof(signalDefs)/sizeof(SignalDefs); ++i) {
                sigaction(signalDefs[i].id, &sa, &oldSigActions[i]);
            }
        }


        ~FatalConditionHandler() {
            reset();
        }
        static void reset() {
            if( isSet ) {
                // Set signals back to previous values -- hopefully nobody overwrote them in the meantime
                for( std::size_t i = 0; i < sizeof(signalDefs)/sizeof(SignalDefs); ++i ) {
                    sigaction(signalDefs[i].id, &oldSigActions[i], CATCH_NULL);
                }
                // Return the old stack
                sigaltstack(&oldSigStack, CATCH_NULL);
                isSet = false;
            }
        }
    };

    bool FatalConditionHandler::isSet = false;
    struct sigaction FatalConditionHandler::oldSigActions[sizeof(signalDefs)/sizeof(SignalDefs)] = {};
    stack_t FatalConditionHandler::oldSigStack = {};
    char FatalConditionHandler::altStackMem[SIGSTKSZ] = {};


} // namespace Catch

#  endif // CATCH_CONFIG_POSIX_SIGNALS

#endif // not Windows

#endif // TWOBLUECUBES_CATCH_FATAL_CONDITION_H_INCLUDED
