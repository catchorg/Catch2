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

// See e.g.:
// https://opensource.apple.com/source/CarbonHeaders/CarbonHeaders-18.1/TargetConditionals.h.auto.html
#ifdef __APPLE__
#  ifndef __has_extension
#    define __has_extension(x) 0
#  endif
#  include <TargetConditionals.h>
#  if (defined(TARGET_OS_OSX) && TARGET_OS_OSX == 1) || \
      (defined(TARGET_OS_MAC) && TARGET_OS_MAC == 1)
#    define CATCH_PLATFORM_MAC
#  elif (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1)
#    define CATCH_PLATFORM_IPHONE
#  endif

#elif defined(linux) || defined(__linux) || defined(__linux__)
#  define CATCH_PLATFORM_LINUX

#elif defined(WIN32) || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER) || defined(__MINGW32__)
#  define CATCH_PLATFORM_WINDOWS
#endif

#endif // TWOBLUECUBES_CATCH_PLATFORM_H_INCLUDED
