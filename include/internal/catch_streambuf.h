/*
 *  Created by Phil on 27/11/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_STREAMBUF_H_INCLUDED
#define TWOBLUECUBES_CATCH_STREAMBUF_H_INCLUDED

#include <streambuf>

namespace Catch {

    class StreamBufBase : public std::streambuf {
    public:
        virtual ~StreamBufBase() throw();
    };
}

#endif // TWOBLUECUBES_CATCH_STREAMBUF_H_INCLUDED
