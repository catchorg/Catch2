/*
 *  catch_objc_main.hpp
 *  OCTest
 *
 *  Created by Phil on 28/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef TWOBLUECUBES_CATCH_OBJC_MAIN_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_OBJC_MAIN_HPP_INCLUDED

#include "catch_runner.hpp"
#include "catch_objc.hpp"

int main (int argc, const char * argv[]) 
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    Catch::registerTestMethods();
    
    int result = Catch::Main( argc, (char* const*)argv );
    
    [pool drain];
    return result;
}

#endif