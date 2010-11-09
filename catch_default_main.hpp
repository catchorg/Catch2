/*
 *  catch_default_main.hpp
 *  Catch
 *
 *  Created by Phil on 01/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED

#include "catch_runner.hpp"
#include "catch.hpp"

int main (int argc, char * const argv[])
{
    return Catch::Main( argc, argv );
}

#endif // TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED