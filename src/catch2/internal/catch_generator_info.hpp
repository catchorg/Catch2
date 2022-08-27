
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_GENERATOR_INFO_HPP_INCLUDED
#define CATCH_GENERATOR_INFO_HPP_INCLUDED

#include <catch2/interfaces/catch_interfaces_capture.hpp>
#include <catch2/internal/catch_source_line_info.hpp>
#include <string>

namespace Catch {

    struct GeneratorInfo {
        GeneratorInfo( StringRef _definition,
                       SourceLineInfo const& _lineInfo,
                       StringRef currentElement );

        StringRef definition;
        SourceLineInfo lineInfo;
        StringRef currentElement;

        bool operator==( GeneratorInfo const& other ) const {
            return definition == other.definition &&
                   lineInfo == other.lineInfo &&
                   currentElement == other.currentElement;
        }
    };

} // end namespace Catch

#endif // CATCH_GENERATOR_INFO_HPP_INCLUDED
