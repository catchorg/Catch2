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

#include "catch_platform.h"
#include "catch_compiler_capabilities.h"
#include "catch_windows_h_proxy.h"


#if defined( CATCH_CONFIG_WINDOWS_SEH )

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

#elif defined ( CATCH_CONFIG_POSIX_SIGNALS )

#include <signal.h>

namespace Catch {

    struct FatalConditionHandler {

        static bool isSet;
        static struct sigaction oldSigActions[];
        static stack_t oldSigStack;
        static char altStackMem[];

        static void handleSignal( int sig );

        FatalConditionHandler();
        ~FatalConditionHandler();
        static void reset();
    };

} // namespace Catch


#else

namespace Catch {
    struct FatalConditionHandler {
        void reset();
    };
}

#endif

#endif // TWOBLUECUBES_CATCH_FATAL_CONDITION_H_INCLUDED
