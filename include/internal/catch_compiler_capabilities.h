/*
 *  Created by Phil on 15/04/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_COMPILER_CAPABILITIES_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_COMPILER_CAPABILITIES_HPP_INCLUDED

// Much of the following code is based on Boost (1.53)

////////////////////////////////////////////////////////////////////////////////
// Borland
#ifdef __BORLANDC__

#if (__BORLANDC__ > 0x582 )
//#define CATCH_CONFIG_SFINAE // Not confirmed
#endif

#endif // __BORLANDC__

////////////////////////////////////////////////////////////////////////////////
// EDG
#ifdef __EDG_VERSION__

#if (__EDG_VERSION__ > 238 )
//#define CATCH_CONFIG_SFINAE // Not confirmed
#endif

#endif // __EDG_VERSION__

////////////////////////////////////////////////////////////////////////////////
// Digital Mars
#ifdef __DMC__

#if (__DMC__ > 0x840 )
//#define CATCH_CONFIG_SFINAE // Not confirmed
#endif

#endif // __DMC__

////////////////////////////////////////////////////////////////////////////////
// GCC
#ifdef __GNUC__

#if __GNUC__ < 3

#if (__GNUC_MINOR__ >= 96 )
//#define CATCH_CONFIG_SFINAE
#endif

#elif __GNUC__ >= 3

// #define CATCH_CONFIG_SFINAE // Taking this out completely for now

#endif // __GNUC__ < 3


#endif // __GNUC__

////////////////////////////////////////////////////////////////////////////////
// Visual C++
#ifdef _MSC_VER

#if (_MSC_VER >= 1310 ) // (VC++ 7.0+)
//#define CATCH_CONFIG_SFINAE // Not confirmed
#endif

#endif // _MSC_VER

// Use variadic macros if the compiler supports them
#if ( defined _MSC_VER && _MSC_VER > 1400 && !defined __EDGE__) || \
    ( defined __WAVE__ && __WAVE_HAS_VARIADICS ) || \
    ( defined __GNUC__ && __GNUC__ >= 3 ) || \
    ( !defined __cplusplus && __STDC_VERSION__ >= 199901L || __cplusplus >= 201103L )

#ifndef CATCH_CONFIG_NO_VARIADIC_MACROS
#define CATCH_CONFIG_VARIADIC_MACROS
#endif

#endif

#endif // TWOBLUECUBES_CATCH_COMPILER_CAPABILITIES_HPP_INCLUDED

