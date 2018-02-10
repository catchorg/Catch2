/*
 *  Created by Phil on 15/04/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_COMPILER_CAPABILITIES_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_COMPILER_CAPABILITIES_HPP_INCLUDED

// Detect a number of compiler features - by compiler
// The following features are defined:
//
// CATCH_CONFIG_COUNTER : is the __COUNTER__ macro supported?
// CATCH_CONFIG_WINDOWS_SEH : is Windows SEH supported?
// CATCH_CONFIG_POSIX_SIGNALS : are POSIX signals supported?
// ****************
// Note to maintainers: if new toggles are added please document them
// in configuration.md, too
// ****************

// In general each macro has a _NO_<feature name> form
// (e.g. CATCH_CONFIG_NO_POSIX_SIGNALS) which disables the feature.
// Many features, at point of detection, define an _INTERNAL_ macro, so they
// can be combined, en-mass, with the _NO_ forms later.


#ifdef __cplusplus

#  if __cplusplus >= 201402L
#    define CATCH_CPP14_OR_GREATER
#  endif

#  if __cplusplus >= 201703L
#    define CATCH_CPP17_OR_GREATER
#  endif

#endif

#if defined(CATCH_CPP17_OR_GREATER)
#  define CATCH_INTERNAL_CONFIG_CPP17_UNCAUGHT_EXCEPTIONS
#endif

#ifdef __clang__

#       define CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
            _Pragma( "clang diagnostic push" ) \
            _Pragma( "clang diagnostic ignored \"-Wexit-time-destructors\"" ) \
            _Pragma( "clang diagnostic ignored \"-Wglobal-constructors\"")
#       define CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS \
            _Pragma( "clang diagnostic pop" )

#       define CATCH_INTERNAL_SUPPRESS_PARENTHESES_WARNINGS \
            _Pragma( "clang diagnostic push" ) \
            _Pragma( "clang diagnostic ignored \"-Wparentheses\"" )
#       define CATCH_INTERNAL_UNSUPPRESS_PARENTHESES_WARNINGS \
            _Pragma( "clang diagnostic pop" )

#endif // __clang__


////////////////////////////////////////////////////////////////////////////////
// We know some environments not to support full POSIX signals
#if defined(__CYGWIN__) || defined(__QNX__) || defined(__EMSCRIPTEN__)

#   if !defined(CATCH_CONFIG_POSIX_SIGNALS)
#       define CATCH_INTERNAL_CONFIG_NO_POSIX_SIGNALS
#   endif

#endif

#ifdef __OS400__
#       define CATCH_INTERNAL_CONFIG_NO_POSIX_SIGNALS
#       define CATCH_CONFIG_COLOUR_NONE
#endif

////////////////////////////////////////////////////////////////////////////////
// Cygwin
#ifdef __CYGWIN__

// Required for some versions of Cygwin to declare gettimeofday
// see: http://stackoverflow.com/questions/36901803/gettimeofday-not-declared-in-this-scope-cygwin
#   define _BSD_SOURCE

#endif // __CYGWIN__

////////////////////////////////////////////////////////////////////////////////
// Visual C++
#ifdef _MSC_VER


#  if _MSC_VER >= 1900 // Visual Studio 2015 or newer
#    define CATCH_INTERNAL_CONFIG_CPP17_UNCAUGHT_EXCEPTIONS
#  endif

// Universal Windows platform does not support SEH
// Or console colours (or console at all...)
#  if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
#    define CATCH_CONFIG_COLOUR_NONE
#  else
#    define CATCH_INTERNAL_CONFIG_WINDOWS_SEH
#  endif

#endif // _MSC_VER

////////////////////////////////////////////////////////////////////////////////

// Use of __COUNTER__ is suppressed during code analysis in
// CLion/AppCode 2017.2.x and former, because __COUNTER__ is not properly
// handled by it.
// Otherwise all supported compilers support COUNTER macro,
// but user still might want to turn it off
#if ( !defined(__JETBRAINS_IDE__) || __JETBRAINS_IDE__ >= 20170300L )
    #define CATCH_INTERNAL_CONFIG_COUNTER
#endif

#if defined(CATCH_INTERNAL_CONFIG_COUNTER) && !defined(CATCH_CONFIG_NO_COUNTER) && !defined(CATCH_CONFIG_COUNTER)
#   define CATCH_CONFIG_COUNTER
#endif
#if defined(CATCH_INTERNAL_CONFIG_WINDOWS_SEH) && !defined(CATCH_CONFIG_NO_WINDOWS_SEH) && !defined(CATCH_CONFIG_WINDOWS_SEH)
#   define CATCH_CONFIG_WINDOWS_SEH
#endif
// This is set by default, because we assume that unix compilers are posix-signal-compatible by default.
#if !defined(CATCH_INTERNAL_CONFIG_NO_POSIX_SIGNALS) && !defined(CATCH_CONFIG_NO_POSIX_SIGNALS) && !defined(CATCH_CONFIG_POSIX_SIGNALS)
#   define CATCH_CONFIG_POSIX_SIGNALS
#endif

#if defined(CATCH_INTERNAL_CONFIG_CPP17_UNCAUGHT_EXCEPTIONS) && !defined(CATCH_INTERNAL_CONFIG_NO_CPP17_UNCAUGHT_EXCEPTIONS) && !defined(CATCH_CONFIG_CPP17_UNCAUGHT_EXCEPTIONS)
#  define CATCH_CONFIG_CPP17_UNCAUGHT_EXCEPTIONS
#endif


#if !defined(CATCH_INTERNAL_SUPPRESS_PARENTHESES_WARNINGS)
#   define CATCH_INTERNAL_SUPPRESS_PARENTHESES_WARNINGS
#   define CATCH_INTERNAL_UNSUPPRESS_PARENTHESES_WARNINGS
#endif
#if !defined(CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS)
#   define CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS
#   define CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS
#endif


#endif // TWOBLUECUBES_CATCH_COMPILER_CAPABILITIES_HPP_INCLUDED

