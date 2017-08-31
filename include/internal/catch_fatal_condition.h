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

#include <string>
#include "catch_platform.h"
#include "catch_compiler_capabilities.h"


#if defined ( CATCH_PLATFORM_WINDOWS ) /////////////////////////////////////////
#include "catch_windows_h_proxy.h"

#  if !defined ( CATCH_CONFIG_WINDOWS_SEH )

namespace Catch {
    struct FatalConditionHandler {
        void reset();
    };
}

#  else // CATCH_CONFIG_WINDOWS_SEH is defined

namespace Catch {

    struct FatalConditionHandler {

        static LONG CALLBACK handleVectoredException(PEXCEPTION_POINTERS ExceptionInfo);
        FatalConditionHandler();
        static void reset();
        ~FatalConditionHandler();

    private:
        static bool isSet;
        static ULONG guaranteeSize;
        static PVOID exceptionHandlerHandle;
    };

} // namespace Catch

#  endif // CATCH_CONFIG_WINDOWS_SEH

#else // Not Windows - assumed to be POSIX compatible //////////////////////////

#  if !defined(CATCH_CONFIG_POSIX_SIGNALS)

namespace Catch {
    struct FatalConditionHandler {
        void reset();
    };
}


#  else // CATCH_CONFIG_POSIX_SIGNALS is defined

#include <signal.h>

namespace Catch {

    struct FatalConditionHandler {

        static bool isSet;
        static struct sigaction oldSigActions[];// [sizeof(signalDefs) / sizeof(SignalDefs)];
        static stack_t oldSigStack;
        static char altStackMem[];

        static void handleSignal( int sig );

        FatalConditionHandler();
        ~FatalConditionHandler();
        static void reset();
    };

} // namespace Catch

#  endif // CATCH_CONFIG_POSIX_SIGNALS

#endif // not Windows

#endif // TWOBLUECUBES_CATCH_FATAL_CONDITION_H_INCLUDED
