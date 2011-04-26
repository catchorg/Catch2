/*
 *  catch_with_main.hpp
 *  Catch
 *
 *  Created by Phil on 01/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_WITH_MAIN_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_WITH_MAIN_HPP_INCLUDED

#include "catch_runner.hpp"
#include "catch.hpp"

int main (int argc, char * const argv[])
{
#ifdef __OBJC__

    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    Catch::registerTestMethods();
    
    int result = Catch::Main( argc, (char* const*)argv );
    
    [pool drain];
    return result;

#else

    return Catch::Main( argc, argv );

#endif
}

#endif // TWOBLUECUBES_CATCH_WITH_MAIN_HPP_INCLUDED
