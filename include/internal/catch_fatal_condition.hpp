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

#include <cassert>
#include <stdexcept>

namespace Catch {

    //! Signals fatal error message to the run context
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
    class FatalConditionHandler {
        bool m_started;

        // Install/disengage implementation for specific platform.
        // Should be if-defed to work on current platform, can assume
        // engage-disengage 1:1 pairing.
        void engage_platform() {}
        void disengage_platform() {}

    public:
        // Should also have platform-specific implementations as needed
        FatalConditionHandler() : m_started(false) {}
        ~FatalConditionHandler() {}

        void engage() {
            assert(!m_started && "Handler cannot be installed twice.");
            m_started = true;
            engage_platform();
        }

        void disengage() {
            assert(m_started && "Handler cannot be uninstalled without being installed first");
            m_started = false;
            disengage_platform();
        }
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

    // Since we do not support multiple instantiations, we put these
    // into global variables and rely on cleaning them up in outlined
    // constructors/destructors
    static PVOID exceptionHandlerHandle = CATCH_NULL;

    class FatalConditionHandler {
        bool m_started;

        // Install/disengage implementation for specific platform.
        // Should be if-defed to work on current platform, can assume
        // engage-disengage 1:1 pairing.

        void engage_platform() {
            // Register as first handler in current chain
            exceptionHandlerHandle = AddVectoredExceptionHandler(1, handleVectoredException);
            if (!exceptionHandlerHandle) {
                throw std::runtime_error("Could not register vectored exception handler");
            }
        }

        void disengage_platform() {
            if (!RemoveVectoredExceptionHandler(exceptionHandlerHandle)) {
                throw std::runtime_error("Could not unregister vectored exception handler");
            }
            exceptionHandlerHandle = CATCH_NULL;
        }

    public:
        FatalConditionHandler() : m_started(false) {
            ULONG guaranteeSize = static_cast<ULONG>(32 * 1024);
            if (!SetThreadStackGuarantee(&guaranteeSize)) {
                // We do not want to fully error out, because needing
                // the stack reserve should be rare enough anyway.
                Catch::cerr()
                    << "Failed to reserve piece of stack."
                    << " Stack overflows will not be reported successfully.";
            }
        }

        // We do not attempt to unset the stack guarantee, because
        // Windows does not support lowering the stack size guarantee.
        ~FatalConditionHandler() {}

        void engage() {
            assert(!m_started && "Handler cannot be installed twice.");
            m_started = true;
            engage_platform();
        }

        void disengage() {
            assert(m_started && "Handler cannot be uninstalled without being installed first");
            m_started = false;
            disengage_platform();
        }
    };

} // namespace Catch

#  endif // CATCH_CONFIG_WINDOWS_SEH

#else // Not Windows - assumed to be POSIX compatible //////////////////////////

#  if !defined(CATCH_CONFIG_POSIX_SIGNALS)

namespace Catch {
    class FatalConditionHandler {
        bool m_started;

        // Install/disengage implementation for specific platform.
        // Should be if-defed to work on current platform, can assume
        // engage-disengage 1:1 pairing.
        void engage_platform() {}
        void disengage_platform() {}

    public:
        // Should also have platform-specific implementations as needed
        FatalConditionHandler() : m_started(false) {}
        ~FatalConditionHandler() {}

        void engage() {
            assert(!m_started && "Handler cannot be installed twice.");
            m_started = true;
            engage_platform();
        }

        void disengage() {
            assert(m_started && "Handler cannot be uninstalled without being installed first");
            m_started = false;
            disengage_platform();
        }
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

// Older GCCs trigger -Wmissing-field-initializers for T foo = {}
// which is zero initialization, but not explicit. We want to avoid
// that.
#if defined(__GNUC__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

    static char* altStackMem = CATCH_NULL;
    static std::size_t altStackSize = 0;
    static stack_t oldSigStack;
    static struct sigaction oldSigActions[sizeof(signalDefs) / sizeof(SignalDefs)];

    static void restorePreviousSignalHandlers() {
        // We set signal handlers back to the previous ones. Hopefully
        // nobody overwrote them in the meantime, and doesn't expect
        // their signal handlers to live past ours given that they
        // installed them after ours..
        for (std::size_t i = 0; i < sizeof(signalDefs) / sizeof(SignalDefs); ++i) {
            sigaction(signalDefs[i].id, &oldSigActions[i], CATCH_NULL);
        }
        // Return the old stack
        sigaltstack(&oldSigStack, CATCH_NULL);
    }

    static void handleSignal( int sig ) {
        char const * name = "<unknown signal>";
        for (std::size_t i = 0; i < sizeof(signalDefs) / sizeof(SignalDefs); ++i) {
            SignalDefs &def = signalDefs[i];
            if (sig == def.id) {
                name = def.name;
                break;
            }
        }
        // We need to restore previous signal handlers and let them do
        // their thing, so that the users can have the debugger break
        // when a signal is raised, and so on.
        restorePreviousSignalHandlers();
        reportFatal( name );
        raise( sig );
    }

    class FatalConditionHandler {
        bool m_started;

        // Install/disengage implementation for specific platform.
        // Should be if-defed to work on current platform, can assume
        // engage-disengage 1:1 pairing.

        void engage_platform() {
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

        void disengage_platform() {
            restorePreviousSignalHandlers();
        }

    public:
        FatalConditionHandler() : m_started(false) {
            assert(!altStackMem && "Cannot initialize POSIX signal handler when one already exists");
            if (altStackSize == 0) {
                altStackSize = SIGSTKSZ;
            }
            altStackMem = new char[altStackSize]();
        }

        ~FatalConditionHandler() {
            delete[] altStackMem;
            // We signal that another instance can be constructed by zeroing
            // out the pointer.
            altStackMem = CATCH_NULL;
        }

        void engage() {
            assert(!m_started && "Handler cannot be installed twice.");
            m_started = true;
            engage_platform();
        }

        void disengage() {
            assert(m_started && "Handler cannot be uninstalled without being installed first");
            m_started = false;
            disengage_platform();
        }
    };

#if defined(__GNUC__)
#    pragma GCC diagnostic pop
#endif

} // namespace Catch

#  endif // CATCH_CONFIG_POSIX_SIGNALS

#endif // not Windows

namespace Catch {

    //! Simple RAII guard for (dis)engaging the FatalConditionHandler
    class FatalConditionHandlerGuard {
        FatalConditionHandler* m_handler;
    public:
        FatalConditionHandlerGuard(FatalConditionHandler* handler):
            m_handler(handler) {
            m_handler->engage();
        }
        ~FatalConditionHandlerGuard() {
            m_handler->disengage();
        }
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_FATAL_CONDITION_H_INCLUDED
