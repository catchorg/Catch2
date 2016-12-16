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

    // Report the error condition then exit the process
    inline void fatal( std::string const& message, int exitCode ) {
        IContext& context = Catch::getCurrentContext();
        IResultCapture* resultCapture = context.getResultCapture();
        resultCapture->handleFatalErrorCondition( message );

		if( Catch::alwaysTrue() ) // avoids "no return" warnings
            exit( exitCode );
    }

} // namespace Catch

#if defined ( CATCH_PLATFORM_WINDOWS ) /////////////////////////////////////////

namespace Catch {

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#undef NOMINMAX


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
                    fatal(signalDefs[i].name, -i);
                }
            }
            // If its not an exception we care about, pass it along.
            // This stops us from eating debugger breaks etc.
            return EXCEPTION_CONTINUE_SEARCH;
        }

        // 32k seems enough for Catch to handle stack overflow,
        // but the value was found experimentally, so there is no strong guarantee
        FatalConditionHandler():m_isSet(true), m_guaranteeSize(32 * 1024) {
            // Register as first handler in current chain
            AddVectoredExceptionHandler(1, handleVectoredException);
            // Pass in guarantee size to be filled
            SetThreadStackGuarantee(&m_guaranteeSize);
        }

        void reset() {
            if (m_isSet) {
                // Unregister handler and restore the old guarantee
                RemoveVectoredExceptionHandler(handleVectoredException);
                SetThreadStackGuarantee(&m_guaranteeSize);
                m_isSet = false;
            }
        }

        ~FatalConditionHandler() {
            reset();
        }
    private:
        bool m_isSet;
        ULONG m_guaranteeSize;
    };

} // namespace Catch

#else // Not Windows - assumed to be POSIX compatible //////////////////////////

#include <signal.h>

namespace Catch {

    struct SignalDefs { int id; const char* name; };
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

        static void handleSignal( int sig ) {
            for( std::size_t i = 0; i < sizeof(signalDefs)/sizeof(SignalDefs); ++i )
                if( sig == signalDefs[i].id )
                    fatal( signalDefs[i].name, -sig );
            fatal( "<unknown signal>", -sig );
        }

        FatalConditionHandler() : m_isSet( true ) {
            for( std::size_t i = 0; i < sizeof(signalDefs)/sizeof(SignalDefs); ++i )
                signal( signalDefs[i].id, handleSignal );
        }
        ~FatalConditionHandler() {
            reset();
        }
        void reset() {
            if( m_isSet ) {
                for( std::size_t i = 0; i < sizeof(signalDefs)/sizeof(SignalDefs); ++i )
                    signal( signalDefs[i].id, SIG_DFL );
                m_isSet = false;
            }
        }

        bool m_isSet;
    };

} // namespace Catch

#endif // not Windows

#endif // TWOBLUECUBES_CATCH_FATAL_CONDITION_H_INCLUDED
