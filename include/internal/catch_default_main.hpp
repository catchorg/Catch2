/*
 *  Created by Phil on 20/05/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED

#include "catch_platform.h"

#ifndef CATCH_PLATFORM_WINDOWS
  #ifndef CATCH_CONFIG_MAIN_ARGV_TYPE
    #define CATCH_CONFIG_MAIN_ARGV_TYPE char * const
  #endif
#else
  #ifndef CATCH_CONFIG_MAIN_ARGV_TYPE
    #define CATCH_CONFIG_MAIN_ARGV_TYPE char *
  #endif
#endif

#ifndef __OBJC__

// Standard C/C++ main entry point
int main (int argc, CATCH_CONFIG_MAIN_ARGV_TYPE argv[]) {
    return Catch::Session().run( argc, argv );
}

#else // __OBJC__

// Objective-C entry point
int main (int argc, CATCH_CONFIG_MAIN_ARGV_TYPE argv[]) {
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
