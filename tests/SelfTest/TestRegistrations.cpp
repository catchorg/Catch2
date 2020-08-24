/*
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <catch2/catch_tag_alias_autoregistrar.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>

// Some example tag aliases
CATCH_REGISTER_TAG_ALIAS( "[@nhf]", "[failing]~[.]" )
CATCH_REGISTER_TAG_ALIAS( "[@tricky]", "[tricky]~[.]" )

#ifdef __clang__
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wweak-vtables"
#   pragma clang diagnostic ignored "-Wc++98-compat"
#endif


struct TestListener : Catch::EventListenerBase {
    using EventListenerBase::EventListenerBase;
};

#include <catch2/catch_reporter_registrars.hpp>

CATCH_REGISTER_LISTENER( TestListener )
