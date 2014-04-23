/*
 *  Created by Fraser on 07/03/2014.
 *  Copyright 2014 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main (int argc, char * const argv[]) {
    return Catch::Session().run( argc, argv );
}
