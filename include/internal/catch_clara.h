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

#define CLARA_CONFIG_CONSOLE_WIDTH CATCH_CONFIG_CONSOLE_WIDTH


// Declare Clara inside the Catch namespace
#define CLICHE_CLARA_OUTER_NAMESPACE Catch
#include "clara.h"
#undef CLICHE_CLARA_OUTER_NAMESPACE

#endif // TWOBLUECUBES_CATCH_CLARA_H_INCLUDED
