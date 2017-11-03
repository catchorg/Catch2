/*
 *  Created by Phil on 13/11/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_VERSION_H_INCLUDED
#define TWOBLUECUBES_CATCH_VERSION_H_INCLUDED

#include <iosfwd>

namespace Catch {

    // Versioning information
    struct Version {
        Version( Version const& ) = delete;
        Version& operator=( Version const& ) = delete;
        Version(    unsigned int _majorVersion,
                    unsigned int _minorVersion,
                    unsigned int _patchNumber,
                    char const * const _branchName,
                    unsigned int _buildNumber );

        unsigned int const majorVersion;
        unsigned int const minorVersion;
        unsigned int const patchNumber;

        // buildNumber is only used if branchName is not null
        char const * const branchName;
        unsigned int const buildNumber;

        friend std::ostream& operator << ( std::ostream& os, Version const& version );
    };

    Version const& libraryVersion();
}

#endif // TWOBLUECUBES_CATCH_VERSION_H_INCLUDED
