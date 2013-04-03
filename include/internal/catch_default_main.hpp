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


#ifndef DO_NOT_USE_SIGNALS
#include <signal.h>
#include <iostream>
#include <sstream>

// handler for signals
void handle_signal(int sig) {
    std::stringstream s;
    s << "\n\n=================\n\n";

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

    // print info about current test
    Catch::IResultCapture& result = Catch::getCurrentContext().getResultCapture();
    const Catch::AssertionResult* r = result.getLastResult();
    s << "\n\nwhile executing test: \"" << result.getCurrentTestName() << "\"\n";
    s << "\n(last successful check was: \"" << r->getExpression() << "\"\n";
    s << " in: " << r->getSourceInfo().file << ", ";
    s << "line: " << static_cast<int>(r->getSourceInfo().line) << ")\n\n";

    std::cout << s.str();

    Catch::cleanUp();
    exit(-sig);
}
#endif /*!DO_NOT_USE_SIGNALS*/

// Standard C/C++ main entry point
int main(int argc, char* const argv[]) {
#ifndef DO_NOT_USE_SIGNALS
    signal(SIGSEGV, handle_signal);
    signal(SIGABRT, handle_signal);
    signal(SIGINT, handle_signal);
    signal(SIGILL, handle_signal);
    signal(SIGFPE, handle_signal);
    signal(SIGSEGV, handle_signal);
    signal(SIGTERM, handle_signal);
    signal(SIGABRT, handle_signal);
#endif /*!DO_NOT_USE_SIGNALS*/

    return Catch::Main(argc, argv);
}

#else // __OBJC__

// Objective-C entry point
int main (int argc, char * const argv[]) {
#if !CATCH_ARC_ENABLED
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
#endif
    
    Catch::registerTestMethods();    
    int result = Catch::Main( argc, (char* const*)argv );
    
#if !CATCH_ARC_ENABLED
    [pool drain];
#endif
    
    return result;
}

#endif // __OBJC__

#endif // TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED
