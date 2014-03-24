/*
 *  Created by Phil on 07/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_INTERFACES_RUNNER_H_INCLUDED
#define TWOBLUECUBES_CATCH_INTERFACES_RUNNER_H_INCLUDED

namespace Catch {
    class TestCase;

    struct IRunner {
        virtual ~IRunner();
    };

    struct NullRunner : public IRunner
    {
    };
}

#endif // TWOBLUECUBES_CATCH_INTERFACES_RUNNER_H_INCLUDED
