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

#ifdef TIMEOUT_FOR_ALL_TESTS_IN_SECONDS
#ifdef __GNUC__
#define USING_TIMEOUTS
#else /*!__GNUC__ = for MSVC pthreads-win32 is needed (
      unless this library will go to C++11 and use std::thread, in which case below could be updated accordingly)*/
#pragma message ("\nWARNING: Specified TIMEOUT_FOR_ALL_TESTS_IN_SECONDS, but it's only supported with GCC compilers..\n")
#endif /*__GNUC__*/
#endif /*TIMEOUT_FOR_ALL_TESTS_IN_SECONDS*/

#if defined(USING_TIMEOUTS) || !defined(DO_NOT_USE_SIGNALS)
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

#ifdef USING_TIMEOUTS
#include <pthread.h>
#ifdef CATCH_PLATFORM_WINDOWS
#define SLEEP(x) Sleep(x)
#else
#include <unistd.h>
#define SLEEP(x) sleep(x/1000)
#endif

void* timeout_thread_fcn(void* arg) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    unsigned int elapsed_s = 0;
    volatile bool& exit_timeout_thread = *static_cast<bool*>(arg);
    while(!exit_timeout_thread) {
        SLEEP(1000);
        elapsed_s++;
        if (elapsed_s == TIMEOUT_FOR_ALL_TESTS_IN_SECONDS)
        {
           std::stringstream info;
           info << "\n\n=================\n\n";
           info << "Test execution timed out";
           info << " (timeout was: " <<  TIMEOUT_FOR_ALL_TESTS_IN_SECONDS << " seconds)\n";
           add_test_info(info);
           std::cerr << info.str();
           exit(-1);
        }
    }
    return NULL;
}
#endif /*!TIMEOUT_FOR_ALL_TESTS_IN_SECONDS*/

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
    int ret = 0;
#ifndef DO_NOT_USE_SIGNALS
    testing_finished = false;
    signal(SIGSEGV, handle_signal);
    signal(SIGABRT, handle_signal);
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);
    signal(SIGILL, handle_signal);
    signal(SIGFPE, handle_signal);
#endif /*!DO_NOT_USE_SIGNALS*/

#ifdef USING_TIMEOUTS
    pthread_t timeout_thread;
    volatile bool exit_timeout_thread = false;
    ret = pthread_create(&timeout_thread,
                         NULL,
                         timeout_thread_fcn,
                         (void*)&exit_timeout_thread);
    if (ret) {
        std::cerr << "Catch failed to start timeout thread (pthread_create failed with %d" << ret << ")\n";
        return ret;
        }
#endif /*!USING_TIMEOUTS*/

    ret = Catch::Session().run( argc, argv );

#ifndef DO_NOT_USE_SIGNALS
    testing_finished = true;
#endif /*!DO_NOT_USE_SIGNALS*/
    return ret;
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


#endif // __OBJC__

#endif // TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED
