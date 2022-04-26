
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_SECTION_INFO_HPP_INCLUDED
#define CATCH_SECTION_INFO_HPP_INCLUDED

#include <catch2/catch_totals.hpp>
#include <catch2/internal/catch_dll_public.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>
#include <catch2/internal/catch_source_line_info.hpp>
#include <catch2/internal/catch_stringref.hpp>
#include <string>

namespace Catch {

    struct CATCH_DLL_PUBLIC SectionInfo {
        // The last argument is ignored, so that people can write
        // SECTION("ShortName", "Proper description that is long") and
        // still use the `-c` flag comfortably.
        SectionInfo( SourceLineInfo const& _lineInfo, std::string _name,
                    const char* const = nullptr ):
            name(CATCH_MOVE(_name)),
            lineInfo(_lineInfo)
            {}

        std::string name;
        SourceLineInfo lineInfo;
    };

    struct CATCH_DLL_PUBLIC SectionEndInfo {
        SectionInfo sectionInfo;
        Counts prevAssertions;
        double durationInSeconds;
    };

} // end namespace Catch

#endif // CATCH_SECTION_INFO_HPP_INCLUDED
