/*
 *  Created by Phil on 2/12/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_STREAM_H_INCLUDED
#define TWOBLUECUBES_CATCH_STREAM_H_INCLUDED

#include <streambuf>

#ifdef __clang__
#pragma clang diagnostic ignored "-Wpadded"
#endif

namespace Catch {

    class Stream {
    public:
        Stream();
        Stream( std::streambuf* _streamBuf, bool _isOwned );
        void release();

        std::streambuf* streamBuf;

    private:
        bool isOwned;
    };
}

#endif // TWOBLUECUBES_CATCH_STREAM_H_INCLUDED
