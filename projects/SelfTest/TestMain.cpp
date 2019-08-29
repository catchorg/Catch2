/*
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// These reporters are not included in the single include, so must be included separately in the main file
#include "reporters/catch_reporter_teamcity.hpp"
#include "reporters/catch_reporter_tap.hpp"
#include "reporters/catch_reporter_automake.hpp"
#include "reporters/catch_reporter_sonarqube.hpp"


// Some example tag aliases
CATCH_REGISTER_TAG_ALIAS( "[@nhf]", "[failing]~[.]" )
CATCH_REGISTER_TAG_ALIAS( "[@tricky]", "[tricky]~[.]" )


#ifdef __clang__
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wweak-vtables"
#   pragma clang diagnostic ignored "-Wc++98-compat"
#endif

struct TestListener : Catch::TestEventListenerBase {
    using TestEventListenerBase::TestEventListenerBase; // inherit constructor
};
CATCH_REGISTER_LISTENER( TestListener )

