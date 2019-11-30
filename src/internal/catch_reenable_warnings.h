/*
 *  Copyright 2014 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef TWOBLUECUBES_CATCH_REENABLE_WARNINGS_H_INCLUDED
#define TWOBLUECUBES_CATCH_REENABLE_WARNINGS_H_INCLUDED

#ifdef __clang__
#    ifdef __ICC // icpc defines the __clang__ macro
#        pragma warning(pop)
#    else
#        pragma clang diagnostic pop
#    endif
#elif defined __GNUC__
#    pragma GCC diagnostic pop
#endif

#endif // TWOBLUECUBES_CATCH_REENABLE_WARNINGS_H_INCLUDED
