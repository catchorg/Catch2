/*
 *  Created by Phil on 5/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_LIST_H_INCLUDED
#define TWOBLUECUBES_CATCH_LIST_H_INCLUDED

#include "catch_config.hpp"

#include <set>
#include <string>


namespace Catch {

    struct IStreamingReporter;

    struct ReporterDescription {
        std::string name, description;
    };

    struct TagInfo {
        void add(std::string const& spelling);
        std::string all() const;

        std::set<std::string> spellings;
        std::size_t count = 0;
    };

    struct testClassInfo {};

    bool list( IStreamingReporter& reporter, std::shared_ptr<Config> const& config );

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_LIST_H_INCLUDED
