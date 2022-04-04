
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_REPORTER_SPEC_PARSER_HPP_INCLUDED
#define CATCH_REPORTER_SPEC_PARSER_HPP_INCLUDED

#include <catch2/internal/catch_optional.hpp>
#include <catch2/internal/catch_stringref.hpp>

#include <vector>
#include <string>

namespace Catch {

    enum class ColourMode : std::uint8_t;

    namespace Detail {
        //! Splits the reporter spec into reporter name and kv-pair options
        std::vector<std::string> splitReporterSpec( StringRef reporterSpec );

        Optional<ColourMode> stringToColourMode( StringRef colourMode );
    }

}

#endif // CATCH_REPORTER_SPEC_PARSER_HPP_INCLUDED
