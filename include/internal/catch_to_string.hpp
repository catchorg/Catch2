/*
 *  Created by Martin on 9/5/2018.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TO_STRING_H_INCLUDED
#define TWOBLUECUBES_CATCH_TO_STRING_H_INCLUDED

#include <string>

#include "catch_compiler_capabilities.h"
#include "catch_stream.h"

namespace Catch {
    template <typename T>
    std::string to_string(T const& t) {
#if defined(CATCH_CONFIG_CPP11_TO_STRING)
        return std::to_string(t);
#else
        ReusableStringStream rss;
        rss << t;
        return rss.str();
#endif
    }
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TO_STRING_H_INCLUDED
