/*
 *  Created by Phil on 5/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_LIST_H_INCLUDED
#define TWOBLUECUBES_CATCH_LIST_H_INCLUDED

#include <vector>
#include <string>

namespace Catch {

    class Config;
    struct IStreamingReporter;

    struct ReporterDescription {
        std::string name;
        std::string description;
    };

    struct TagInfo {
        void add(std::string const& spelling);
        std::string tagsAsString() const;

        std::size_t count = 0;
        std::vector<std::string> spellings;
    };

    bool list( IStreamingReporter& reporter, Config const& config );

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_LIST_H_INCLUDED
