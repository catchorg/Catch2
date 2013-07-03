/*
 *  Created by Phil on 05/06/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_INTERFACES_CONFIG_H_INCLUDED
#define TWOBLUECUBES_CATCH_INTERFACES_CONFIG_H_INCLUDED

#include <iostream>
#include <string>

#include "catch_ptr.hpp"

namespace Catch {

    struct IConfig : IShared {

        virtual ~IConfig();

        virtual bool allowThrows() const = 0;
        virtual std::ostream& stream() const = 0;
        virtual std::string name() const = 0;
        virtual bool includeSuccessfulResults() const = 0;
        virtual bool shouldDebugBreak() const = 0;
        virtual bool warnAboutMissingAssertions() const = 0;
        virtual int abortAfter() const = 0;
    };
}

#endif // TWOBLUECUBES_CATCH_INTERFACES_CONFIG_H_INCLUDED
