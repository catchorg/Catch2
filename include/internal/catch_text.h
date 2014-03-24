/*
 *  Created by Phil on 10/2/2014.
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEXT_H_INCLUDED
#define TWOBLUECUBES_CATCH_TEXT_H_INCLUDED

#include "catch_config.hpp"

#define TBC_TEXT_FORMAT_CONSOLE_WIDTH CATCH_CONFIG_CONSOLE_WIDTH

#define CLICHE_TBC_TEXT_FORMAT_OUTER_NAMESPACE Catch
#include "../external/tbc_text_format.h"
#undef CLICHE_TBC_TEXT_FORMAT_OUTER_NAMESPACE

namespace Catch {
    using Tbc::Text;
    using Tbc::TextAttributes;
}

#endif // TWOBLUECUBES_CATCH_TEXT_H_INCLUDED
