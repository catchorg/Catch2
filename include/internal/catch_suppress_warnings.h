/*
 *  Copyright 2014 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef TWOBLUECUBES_CATCH_SUPPRESS_WARNINGS_H_INCLUDED
#define TWOBLUECUBES_CATCH_SUPPRESS_WARNINGS_H_INCLUDED

#ifdef __clang__
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wvariadic-macros"
#pragma clang diagnostic ignored "-Wc99-extensions"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#endif

#endif // TWOBLUECUBES_CATCH_SUPPRESS_WARNINGS_H_INCLUDED
