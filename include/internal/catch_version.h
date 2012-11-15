/*
 *  Created by Phil on 13/11/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_VERSION_H_INCLUDED
#define TWOBLUECUBES_CATCH_VERSION_H_INCLUDED

namespace Catch {

    // Versioning information
    struct Version {
        const unsigned int MajorVersion;
        const unsigned int MinorVersion;
        const unsigned int BuildNumber;
        const std::string BranchName;
    };

    extern Version libraryVersion;
}

#endif // TWOBLUECUBES_CATCH_VERSION_H_INCLUDED
