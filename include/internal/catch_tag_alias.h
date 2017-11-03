/*
 *  Created by Phil on 27/6/2014.
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TAG_ALIAS_H_INCLUDED
#define TWOBLUECUBES_CATCH_TAG_ALIAS_H_INCLUDED

#include "catch_common.h"

#include <string>

namespace Catch {

    struct TagAlias {
        TagAlias(std::string const& _tag, SourceLineInfo _lineInfo);

        std::string tag;
        SourceLineInfo lineInfo;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TAG_ALIAS_H_INCLUDED
