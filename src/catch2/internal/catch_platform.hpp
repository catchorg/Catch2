#ifndef CATCH_PLATFORM_HPP_INCLUDED
#define CATCH_PLATFORM_HPP_INCLUDED

#ifdef __APPLE__
# include <TargetConditionals.h>
# if TARGET_OS_OSX == 1
#  define CATCH_PLATFORM_MAC
# elif TARGET_OS_IPHONE == 1
#  define CATCH_PLATFORM_IPHONE
# endif

#elif defined(linux) || defined(__linux) || defined(__linux__)
#  define CATCH_PLATFORM_LINUX

#elif defined(WIN32) || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER) || defined(__MINGW32__)
#  define CATCH_PLATFORM_WINDOWS
#endif

#endif // CATCH_PLATFORM_HPP_INCLUDED
