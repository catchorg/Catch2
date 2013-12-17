/*
 *  Created by Phil on 20/05/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED

#ifndef __OBJC__

#if !defined(DO_NOT_USE_SIGNALS)
#include <sstream>
#include <iostream>
void add_test_info(std::stringstream& s) {
    // add info about current test
    Catch::IContext& context = Catch::getCurrentContext();
    Catch::IResultCapture& result = context.getResultCapture();
    const Catch::AssertionResult* ar = result.getLastResult();
    if(ar) {
        Catch::SourceLineInfo src = ar->getSourceInfo();
        s << "\n\nwhile executing test: \"" << result.getCurrentTestName() << "\"\n";
        s << "\n(last successful check was: \"" << ar->getExpression() << "\"\n";
        s << " in: " << src.file << ", ";
        s << "line: " << static_cast<int>(src.line) << ")\n\n";
    }
    Catch::cleanUp();
}
#endif

#ifndef DO_NOT_USE_SIGNALS
static bool testing_finished = false;
#include <signal.h>

// handler for signals
void handle_signal(int sig) {
    std::stringstream s;
    s << "\n\n=================\n\n";
    if(testing_finished)
    {
        std::cerr << s.str() << "received signal " << std::dec << sig;
        std::cerr << " after testing was finished\n";
        exit(0);
    }

    switch (sig) {
    case SIGINT:
        s << "Interactive attention";
        break;
    case SIGILL:
        s << "Illegal instruction";
        break;
    case SIGFPE:
        s << "Floating point error";
        break;
    case SIGSEGV:
        s << "Segmentation violation";
        break;
    case SIGTERM:
        s << "Termination request";
        break;
    case SIGABRT:
        s << "Abnormal termination (abort)";
        break;
    default:
        break;
    }

    add_test_info(s);
    std::cout << s.str();

    exit(-sig);
}
#endif /*!DO_NOT_USE_SIGNALS*/

// Standard C/C++ main entry point
int main (int argc, char * const argv[]) {
#ifndef DO_NOT_USE_SIGNALS
    testing_finished = false;
    signal(SIGSEGV, handle_signal);
    signal(SIGABRT, handle_signal);
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);
    signal(SIGILL, handle_signal);
    signal(SIGFPE, handle_signal);
#endif /*!DO_NOT_USE_SIGNALS*/

    return Catch::Session().run( argc, argv );
}

#else // __OBJC__

// Objective-C entry point
int main (int argc, char * const argv[]) {
#if !CATCH_ARC_ENABLED
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
#endif

    Catch::registerTestMethods();
    int result = Catch::Session().run( argc, (char* const*)argv );

#if !CATCH_ARC_ENABLED
    [pool drain];
#endif

    return result;
}

#endif // __OBJC__

#endif // TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED
