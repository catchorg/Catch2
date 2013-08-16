/*
 *  Created by Phil on 16/8/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_PLATFORM_H_INCLUDED
#define TWOBLUECUBES_CATCH_PLATFORM_H_INCLUDED

#if defined(__MAC_OS_X_VERSION_MIN_REQUIRED)
#define CATCH_PLATFORM_MAC
#elif  defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
#define CATCH_PLATFORM_IPHONE
#elif defined(WIN32) || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER)
#define CATCH_PLATFORM_WINDOWS
#endif

#endif // TWOBLUECUBES_CATCH_PLATFORM_H_INCLUDED
