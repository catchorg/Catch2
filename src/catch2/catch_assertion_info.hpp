/*
 *  Created by Phil on 8/8/2017.
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_ASSERTIONINFO_H_INCLUDED
#define TWOBLUECUBES_CATCH_ASSERTIONINFO_H_INCLUDED

#include <catch2/internal/catch_result_type.hpp>
#include <catch2/internal/catch_common.hpp>
#include <catch2/catch_stringref.hpp>

namespace Catch {

    struct AssertionInfo {
        // AssertionInfo() = delete;

        StringRef macroName;
        SourceLineInfo lineInfo;
        StringRef capturedExpression;
        ResultDisposition::Flags resultDisposition;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_ASSERTIONINFO_H_INCLUDED
