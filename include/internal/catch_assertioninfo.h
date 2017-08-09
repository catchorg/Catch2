/*
 *  Created by Phil on 8/8/2017.
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_ASSERTIONINFO_H_INCLUDED
#define TWOBLUECUBES_CATCH_ASSERTIONINFO_H_INCLUDED

#include "catch_result_type.h"
#include "catch_common.h"
#include "catch_stringref.h"

namespace Catch {

    struct AssertionInfo
    {
        StringRef macroName;
        SourceLineInfo lineInfo;
        StringRef capturedExpression;
        ResultDisposition::Flags resultDisposition;

        // We want to delete this constructor but a compiler bug in 4.8 means
        // the struct is then treated as non-aggregate
        //AssertionInfo() = delete;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_ASSERTIONINFO_H_INCLUDED
