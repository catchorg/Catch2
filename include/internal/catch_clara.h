/*
 *  Created by Phil on 10/2/2014.
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_CLARA_H_INCLUDED
#define TWOBLUECUBES_CATCH_CLARA_H_INCLUDED

// Use Catch's value for console width (store Clara's off to the side, if present)
#ifdef CLARA_CONFIG_CONSOLE_WIDTH
#define CATCH_TEMP_CLARA_CONFIG_CONSOLE_WIDTH CATCH_CLARA_TEXTFLOW_CONFIG_CONSOLE_WIDTH
#undef CATCH_CLARA_TEXTFLOW_CONFIG_CONSOLE_WIDTH
#endif
#define CATCH_CLARA_TEXTFLOW_CONFIG_CONSOLE_WIDTH CATCH_CONFIG_CONSOLE_WIDTH-1

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wshadow"
#endif

#include "../external/clara.hpp"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

// Restore Clara's value for console width, if present
#ifdef CATCH_TEMP_CLARA_CONFIG_CONSOLE_WIDTH
#define CATCH_CLARA_TEXTFLOW_CONFIG_CONSOLE_WIDTH CATCH_TEMP_CLARA_CONFIG_CONSOLE_WIDTH
#undef CATCH_TEMP_CLARA_CONFIG_CONSOLE_WIDTH
#endif

#endif // TWOBLUECUBES_CATCH_CLARA_H_INCLUDED
