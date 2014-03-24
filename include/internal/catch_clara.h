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
#define CATCH_TEMP_CLARA_CONFIG_CONSOLE_WIDTH CLARA_CONFIG_CONSOLE_WIDTH
#undef CLARA_CONFIG_CONSOLE_WIDTH
#endif
#define CLARA_CONFIG_CONSOLE_WIDTH CATCH_CONFIG_CONSOLE_WIDTH


// Declare Clara inside the Catch namespace
#define STITCH_CLARA_OPEN_NAMESPACE namespace Catch {
#include "../external/clara.h"
#undef STITCH_CLARA_OPEN_NAMESPACE


// Restore Clara's value for console width, if present
#ifdef CATCH_TEMP_CLARA_CONFIG_CONSOLE_WIDTH
#define CLARA_CONFIG_CONSOLE_WIDTH CATCH_TEMP_CLARA_CONFIG_CONSOLE_WIDTH
#undef CATCH_TEMP_CLARA_CONFIG_CONSOLE_WIDTH
#endif

#endif // TWOBLUECUBES_CATCH_CLARA_H_INCLUDED
