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

    struct FatalConditionHandler {
		void reset() {}
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

        FatalConditionHandler(): m_isSet(true), m_altStackMem(new char[SIGSTKSZ]) {
            stack_t sigStack;
            sigStack.ss_sp = m_altStackMem;
            sigStack.ss_size = SIGSTKSZ;
            sigStack.ss_flags = 0;
            sigaltstack(&sigStack, &m_oldSigStack);
            struct sigaction sa = { 0 };

            sa.sa_handler = handleSignal;
            sa.sa_flags = SA_ONSTACK;
            for (std::size_t i = 0; i < sizeof(signalDefs)/sizeof(SignalDefs); ++i) {
                sigaction(signalDefs[i].id, &sa, &m_oldSigActions[i]);
            }
        }
        ~FatalConditionHandler() {
            reset();
            delete[] m_altStackMem;
        }
        void reset() {
            if( m_isSet ) {
                // Set signals back to previous values -- hopefully nobody overwrote them in the meantime
                for( std::size_t i = 0; i < sizeof(signalDefs)/sizeof(SignalDefs); ++i ) {
                    sigaction(signalDefs[i].id, &m_oldSigActions[i], CATCH_NULL);
                }
                // Return the old stack
                sigaltstack(&m_oldSigStack, CATCH_NULL);
                m_isSet = false;
            }
        }

        bool m_isSet;
        // C++03 doesn't allow auto_ptr<T[]>, so we have manage the memory ourselves
        char* m_altStackMem;
        struct sigaction m_oldSigActions [sizeof(signalDefs)/sizeof(SignalDefs)];
        stack_t m_oldSigStack;
    };

} // namespace Catch

#endif // not Windows

#endif // TWOBLUECUBES_CATCH_FATAL_CONDITION_H_INCLUDED
