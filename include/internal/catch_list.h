/*
 *  Created by Phil on 5/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_LIST_H_INCLUDED
#define TWOBLUECUBES_CATCH_LIST_H_INCLUDED

#include "catch_option.hpp"
#include "catch_config.hpp"

#include <set>

namespace Catch {

    std::size_t listTests( Config const& config );

    std::size_t listTestsNamesOnly( Config const& config );

    struct TagInfo {
        void add( std::string const& spelling );
        std::string all() const;

        std::set<std::string> spellings;
        std::size_t count = 0;
    };

    std::size_t listTags( Config const& config );

    std::size_t listReporters( Config const& /*config*/ );
    
    Option<std::size_t> list( Config const& config );

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_LIST_H_INCLUDED
