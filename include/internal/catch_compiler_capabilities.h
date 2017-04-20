/*
 *  Created by Phil on 15/04/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_COMPILER_CAPABILITIES_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_COMPILER_CAPABILITIES_HPP_INCLUDED

// Detect a number of compiler features - mostly C++11/14 conformance - by compiler
// The following features are defined:
//
// CATCH_CONFIG_CPP11_NULLPTR : is nullptr supported?
// CATCH_CONFIG_CPP11_NOEXCEPT : is noexcept supported?
// CATCH_CONFIG_CPP11_GENERATED_METHODS : The delete and default keywords for compiler generated methods
// CATCH_CONFIG_CPP11_IS_ENUM : std::is_enum is supported?
// CATCH_CONFIG_CPP11_TUPLE : std::tuple is supported
// CATCH_CONFIG_CPP11_LONG_LONG : is long long supported?
// CATCH_CONFIG_CPP11_OVERRIDE : is override supported?
// CATCH_CONFIG_CPP11_UNIQUE_PTR : is unique_ptr supported (otherwise use auto_ptr)
// CATCH_CONFIG_CPP11_SHUFFLE : is std::shuffle supported?
// CATCH_CONFIG_CPP11_TYPE_TRAITS : are type_traits and enable_if supported?

// CATCH_CONFIG_CPP11_OR_GREATER : Is C++11 supported?

// CATCH_CONFIG_VARIADIC_MACROS : are variadic macros supported?
// CATCH_CONFIG_COUNTER : is the __COUNTER__ macro supported?
// CATCH_CONFIG_WINDOWS_SEH : is Windows SEH supported?
// CATCH_CONFIG_POSIX_SIGNALS : are POSIX signals supported?
// ****************
// Note to maintainers: if new toggles are added please document them
// in configuration.md, too
// ****************

// In general each macro has a _NO_<feature name> form
// (e.g. CATCH_CONFIG_CPP11_NO_NULLPTR) which disables the feature.
// Many features, at point of detection, define an _INTERNAL_ macro, so they
// can be combined, en-mass, with the _NO_ forms later.

// All the C++11 features can be disabled with CATCH_CONFIG_NO_CPP11

#ifdef __cplusplus

#  if __cplusplus >= 201103L
#    define CATCH_CPP11_OR_GREATER
#  endif

#  if __cplusplus >= 201402L
#    define CATCH_CPP14_OR_GREATER
#  endif

#endif

#ifdef __clang__

#  if __has_feature(cxx_nullptr)
#    define CATCH_INTERNAL_CONFIG_CPP11_NULLPTR
#  endif

#  if __has_feature(cxx_noexcept)
#    define CATCH_INTERNAL_CONFIG_CPP11_NOEXCEPT
#  endif

#   if defined(CATCH_CPP11_OR_GREATER)
#       define CATCH_INTERNAL_SUPPRESS_ETD_WARNINGS \
            _Pragma( "clang diagnostic push" ) \
            _Pragma( "clang diagnostic ignored \"-Wexit-time-destructors\"" )
#       define CATCH_INTERNAL_UNSUPPRESS_ETD_WARNINGS \
            _Pragma( "clang diagnostic pop" )

#       define CATCH_INTERNAL_SUPPRESS_PARENTHESES_WARNINGS \
            _Pragma( "clang diagnostic push" ) \
            _Pragma( "clang diagnostic ignored \"-Wparentheses\"" )
#       define CATCH_INTERNAL_UNSUPPRESS_PARENTHESES_WARNINGS \
            _Pragma( "clang diagnostic pop" )
#   endif

#endif // __clang__


////////////////////////////////////////////////////////////////////////////////
// We know some environments not to support full POSIX signals
#if defined(__CYGWIN__) || defined(__QNX__)

#   if !defined(CATCH_CONFIG_POSIX_SIGNALS)
#       define CATCH_INTERNAL_CONFIG_NO_POSIX_SIGNALS
#   endif

#endif


////////////////////////////////////////////////////////////////////////////////
// Cygwin
#ifdef __CYGWIN__

// Required for some versions of Cygwin to declare gettimeofday
// see: http://stackoverflow.com/questions/36901803/gettimeofday-not-declared-in-this-scope-cygwin
#   define _BSD_SOURCE

#endif // __CYGWIN__

////////////////////////////////////////////////////////////////////////////////
// Borland
#ifdef __BORLANDC__


#endif // __BORLANDC__

////////////////////////////////////////////////////////////////////////////////
// EDG
#ifdef __EDG_VERSION__


#endif // __EDG_VERSION__

////////////////////////////////////////////////////////////////////////////////
// Digital Mars
#ifdef __DMC__


#endif // __DMC__

////////////////////////////////////////////////////////////////////////////////
// GCC
#ifdef __GNUC__

#   if __GNUC__ == 4 && __GNUC_MINOR__ >= 6 && defined(__GXX_EXPERIMENTAL_CXX0X__)
#       define CATCH_INTERNAL_CONFIG_CPP11_NULLPTR
#   endif


// - otherwise more recent versions define __cplusplus >= 201103L
// and will get picked up below


#endif // __GNUC__

////////////////////////////////////////////////////////////////////////////////
// Visual C++
#ifdef _MSC_VER

#define CATCH_INTERNAL_CONFIG_WINDOWS_SEH

#if (_MSC_VER >= 1600)
#   define CATCH_INTERNAL_CONFIG_CPP11_NULLPTR
#   define CATCH_INTERNAL_CONFIG_CPP11_UNIQUE_PTR
#endif

#if (_MSC_VER >= 1900 ) // (VC++ 13 (VS2015))
#define CATCH_INTERNAL_CONFIG_CPP11_NOEXCEPT
#define CATCH_INTERNAL_CONFIG_CPP11_GENERATED_METHODS
#define CATCH_INTERNAL_CONFIG_CPP11_SHUFFLE
#define CATCH_INTERNAL_CONFIG_CPP11_TYPE_TRAITS
#endif

#endif // _MSC_VER

////////////////////////////////////////////////////////////////////////////////

// Use variadic macros if the compiler supports them
#if ( defined _MSC_VER && _MSC_VER > 1400 && !defined __EDGE__) || \
    ( defined __WAVE__ && __WAVE_HAS_VARIADICS ) || \
    ( defined __GNUC__ && __GNUC__ >= 3 ) || \
    ( !defined __cplusplus && __STDC_VERSION__ >= 199901L || __cplusplus >= 201103L )

#define CATCH_INTERNAL_CONFIG_VARIADIC_MACROS

#endif

// Use __COUNTER__ if the compiler supports it
#if ( defined _MSC_VER && _MSC_VER >= 1300 ) || \
    ( defined __GNUC__  && __GNUC__ >= 4 && __GNUC_MINOR__ >= 3 ) || \
    ( defined __clang__ && __clang_major__ >= 3 )

#define CATCH_INTERNAL_CONFIG_COUNTER

#endif

////////////////////////////////////////////////////////////////////////////////
// C++ language feature support

// catch all support for C++11
#if defined(CATCH_CPP11_OR_GREATER)

#  if !defined(CATCH_INTERNAL_CONFIG_CPP11_NULLPTR)
#    define CATCH_INTERNAL_CONFIG_CPP11_NULLPTR
#  endif

#  ifndef CATCH_INTERNAL_CONFIG_CPP11_NOEXCEPT
#    define CATCH_INTERNAL_CONFIG_CPP11_NOEXCEPT
#  endif

#  ifndef CATCH_INTERNAL_CONFIG_CPP11_GENERATED_METHODS
#    define CATCH_INTERNAL_CONFIG_CPP11_GENERATED_METHODS
#  endif

#  ifndef CATCH_INTERNAL_CONFIG_CPP11_IS_ENUM
#    define CATCH_INTERNAL_CONFIG_CPP11_IS_ENUM
#  endif

#  ifndef CATCH_INTERNAL_CONFIG_CPP11_TUPLE
#    define CATCH_INTERNAL_CONFIG_CPP11_TUPLE
#  endif

#  ifndef CATCH_INTERNAL_CONFIG_VARIADIC_MACROS
#    define CATCH_INTERNAL_CONFIG_VARIADIC_MACROS
#  endif

#  if !defined(CATCH_INTERNAL_CONFIG_CPP11_LONG_LONG)
#    define CATCH_INTERNAL_CONFIG_CPP11_LONG_LONG
#  endif

#  if !defined(CATCH_INTERNAL_CONFIG_CPP11_OVERRIDE)
#    define CATCH_INTERNAL_CONFIG_CPP11_OVERRIDE
#  endif
#  if !defined(CATCH_INTERNAL_CONFIG_CPP11_UNIQUE_PTR)
#    define CATCH_INTERNAL_CONFIG_CPP11_UNIQUE_PTR
#  endif
# if !defined(CATCH_INTERNAL_CONFIG_CPP11_SHUFFLE)
#   define CATCH_INTERNAL_CONFIG_CPP11_SHUFFLE
#  endif
# if !defined(CATCH_INTERNAL_CONFIG_CPP11_TYPE_TRAITS)
#  define CATCH_INTERNAL_CONFIG_CPP11_TYPE_TRAITS
# endif

#endif // __cplusplus >= 201103L

// Now set the actual defines based on the above + anything the user has configured
#if defined(CATCH_INTERNAL_CONFIG_CPP11_NULLPTR) && !defined(CATCH_CONFIG_CPP11_NO_NULLPTR) && !defined(CATCH_CONFIG_CPP11_NULLPTR) && !defined(CATCH_CONFIG_NO_CPP11)
#   define CATCH_CONFIG_CPP11_NULLPTR
#endif
#if defined(CATCH_INTERNAL_CONFIG_CPP11_NOEXCEPT) && !defined(CATCH_CONFIG_CPP11_NO_NOEXCEPT) && !defined(CATCH_CONFIG_CPP11_NOEXCEPT) && !defined(CATCH_CONFIG_NO_CPP11)
#   define CATCH_CONFIG_CPP11_NOEXCEPT
#endif
#if defined(CATCH_INTERNAL_CONFIG_CPP11_GENERATED_METHODS) && !defined(CATCH_CONFIG_CPP11_NO_GENERATED_METHODS) && !defined(CATCH_CONFIG_CPP11_GENERATED_METHODS) && !defined(CATCH_CONFIG_NO_CPP11)
#   define CATCH_CONFIG_CPP11_GENERATED_METHODS
#endif
#if defined(CATCH_INTERNAL_CONFIG_CPP11_IS_ENUM) && !defined(CATCH_CONFIG_CPP11_NO_IS_ENUM) && !defined(CATCH_CONFIG_CPP11_IS_ENUM) && !defined(CATCH_CONFIG_NO_CPP11)
#   define CATCH_CONFIG_CPP11_IS_ENUM
#endif
#if defined(CATCH_INTERNAL_CONFIG_CPP11_TUPLE) && !defined(CATCH_CONFIG_CPP11_NO_TUPLE) && !defined(CATCH_CONFIG_CPP11_TUPLE) && !defined(CATCH_CONFIG_NO_CPP11)
#   define CATCH_CONFIG_CPP11_TUPLE
#endif
#if defined(CATCH_INTERNAL_CONFIG_VARIADIC_MACROS) && !defined(CATCH_CONFIG_NO_VARIADIC_MACROS) && !defined(CATCH_CONFIG_VARIADIC_MACROS)
#   define CATCH_CONFIG_VARIADIC_MACROS
#endif
#if defined(CATCH_INTERNAL_CONFIG_CPP11_LONG_LONG) && !defined(CATCH_CONFIG_CPP11_NO_LONG_LONG) && !defined(CATCH_CONFIG_CPP11_LONG_LONG) && !defined(CATCH_CONFIG_NO_CPP11)
#   define CATCH_CONFIG_CPP11_LONG_LONG
#endif
#if defined(CATCH_INTERNAL_CONFIG_CPP11_OVERRIDE) && !defined(CATCH_CONFIG_CPP11_NO_OVERRIDE) && !defined(CATCH_CONFIG_CPP11_OVERRIDE) && !defined(CATCH_CONFIG_NO_CPP11)
#   define CATCH_CONFIG_CPP11_OVERRIDE
#endif
#if defined(CATCH_INTERNAL_CONFIG_CPP11_UNIQUE_PTR) && !defined(CATCH_CONFIG_CPP11_NO_UNIQUE_PTR) && !defined(CATCH_CONFIG_CPP11_UNIQUE_PTR) && !defined(CATCH_CONFIG_NO_CPP11)
#   define CATCH_CONFIG_CPP11_UNIQUE_PTR
#endif
// Use of __COUNTER__ is suppressed if __JETBRAINS_IDE__ is #defined (meaning we're being parsed by a JetBrains IDE for
// analytics) because, at time of writing, __COUNTER__ is not properly handled by it.
// This does not affect compilation
#if defined(CATCH_INTERNAL_CONFIG_COUNTER) && !defined(CATCH_CONFIG_NO_COUNTER) && !defined(CATCH_CONFIG_COUNTER) && !defined(__JETBRAINS_IDE__)
#   define CATCH_CONFIG_COUNTER
#endif
#if defined(CATCH_INTERNAL_CONFIG_CPP11_SHUFFLE) && !defined(CATCH_CONFIG_CPP11_NO_SHUFFLE) && !defined(CATCH_CONFIG_CPP11_SHUFFLE) && !defined(CATCH_CONFIG_NO_CPP11)
#   define CATCH_CONFIG_CPP11_SHUFFLE
#endif
# if defined(CATCH_INTERNAL_CONFIG_CPP11_TYPE_TRAITS) && !defined(CATCH_CONFIG_CPP11_NO_TYPE_TRAITS) && !defined(CATCH_CONFIG_CPP11_TYPE_TRAITS) && !defined(CATCH_CONFIG_NO_CPP11)
#  define CATCH_CONFIG_CPP11_TYPE_TRAITS
# endif
#if defined(CATCH_INTERNAL_CONFIG_WINDOWS_SEH) && !defined(CATCH_CONFIG_NO_WINDOWS_SEH) && !defined(CATCH_CONFIG_WINDOWS_SEH)
#   define CATCH_CONFIG_WINDOWS_SEH
#endif
// This is set by default, because we assume that unix compilers are posix-signal-compatible by default.
#if !defined(CATCH_INTERNAL_CONFIG_NO_POSIX_SIGNALS) && !defined(CATCH_CONFIG_NO_POSIX_SIGNALS) && !defined(CATCH_CONFIG_POSIX_SIGNALS)
#   define CATCH_CONFIG_POSIX_SIGNALS
#endif

#if !defined(CATCH_INTERNAL_SUPPRESS_PARENTHESES_WARNINGS)
#   define CATCH_INTERNAL_SUPPRESS_PARENTHESES_WARNINGS
#   define CATCH_INTERNAL_UNSUPPRESS_PARENTHESES_WARNINGS
#endif
#if !defined(CATCH_INTERNAL_SUPPRESS_ETD_WARNINGS)
#   define CATCH_INTERNAL_SUPPRESS_ETD_WARNINGS
#   define CATCH_INTERNAL_UNSUPPRESS_ETD_WARNINGS
#endif

// noexcept support:
#if defined(CATCH_CONFIG_CPP11_NOEXCEPT) && !defined(CATCH_NOEXCEPT)
#  define CATCH_NOEXCEPT noexcept
#  define CATCH_NOEXCEPT_IS(x) noexcept(x)
#else
#  define CATCH_NOEXCEPT throw()
#  define CATCH_NOEXCEPT_IS(x)
#endif

// nullptr support
#ifdef CATCH_CONFIG_CPP11_NULLPTR
#   define CATCH_NULL nullptr
#else
#   define CATCH_NULL NULL
#endif

// override support
#ifdef CATCH_CONFIG_CPP11_OVERRIDE
#   define CATCH_OVERRIDE override
#else
#   define CATCH_OVERRIDE
#endif

// unique_ptr support
#ifdef CATCH_CONFIG_CPP11_UNIQUE_PTR
#   define CATCH_AUTO_PTR( T ) std::unique_ptr<T>
#else
#   define CATCH_AUTO_PTR( T ) std::auto_ptr<T>
#endif

#endif // TWOBLUECUBES_CATCH_COMPILER_CAPABILITIES_HPP_INCLUDED

