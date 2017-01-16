/*
 *  Created by Martin on 16/01/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef TWOBLUECUBES_CATCH_WINDOWS_H_PROXY_H_INCLUDED
#define TWOBLUECUBES_CATCH_WINDOWS_H_PROXY_H_INCLUDED

#ifdef CATCH_DEFINES_NOMINMAX
#  define NOMINMAX
#endif
#ifdef CATCH_DEFINES_WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif

#ifdef __AFXDLL
#include <AfxWin.h>
#else
#include <windows.h>
#endif

#ifdef CATCH_DEFINES_NOMINMAX
#  undef NOMINMAX
#endif
#ifdef CATCH_DEFINES_WIN32_LEAN_AND_MEAN
#  undef WIN32_LEAN_AND_MEAN
#endif


#endif // TWOBLUECUBES_CATCH_WINDOWS_H_PROXY_H_INCLUDED
