/*
 *  Created by Phil on 03/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_SECTION_INFO_H_INCLUDED
#define TWOBLUECUBES_CATCH_SECTION_INFO_H_INCLUDED

#include "catch_common.h"
#include "catch_stringref.h"
#include "catch_totals.h"

#include <string>

namespace Catch {

    struct SectionInfo {
        SectionInfo
            (   SourceLineInfo const& _lineInfo,
                std::string const& _name );

        // The last argument is ignored, so that people can write
        // SECTION("ShortName", "Proper description that is long") and
        // still use the `-c` flag comfortably.
        SectionInfo
            (   SourceLineInfo const& _lineInfo,
                std::string const& _name,
                const char* const ) : SectionInfo( _lineInfo, _name ) {}

        std::string name;
        SourceLineInfo lineInfo;
    };

    struct SectionEndInfo {
        SectionInfo sectionInfo;
        Counts prevAssertions;
        double durationInSeconds;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_SECTION_INFO_H_INCLUDED
