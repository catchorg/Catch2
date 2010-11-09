/*
 *  catch_common.h
 *  Catch
 *
 *  Created by Phil on 29/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef TWOBLUECUBES_CATCH_COMMON_H_INCLUDED
#define TWOBLUECUBES_CATCH_COMMON_H_INCLUDED

#define _CATCH_UNIQUE_NAME_LINE2( name, line ) name##line
#define _CATCH_UNIQUE_NAME_LINE( name, line ) _CATCH_UNIQUE_NAME_LINE2( name, line )
#define _CATCH_UNIQUE_NAME( name ) _CATCH_UNIQUE_NAME_LINE( name, __LINE__ )

#endif // TWOBLUECUBES_CATCH_COMMON_H_INCLUDED