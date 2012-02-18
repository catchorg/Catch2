/*
 *  catch_default_main.hpp
 *  Catch
 *
 *  Created by Phil on 20/05/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED

int main (int argc, char * const argv[])
{
#ifdef __OBJC__
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    Catch::registerTestMethods();
    
    int result = Catch::Main( argc, (char* const*)argv );
    
    [pool drain];
    
#else
    
    int result =Catch::Main( argc, argv );
    
#endif
    Catch::Hub::cleanUp();
    return result;
}

#endif // TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED
