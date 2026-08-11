
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_GENERATOR_FILTER_PARSER_HPP_INCLUDED
#define CATCH_GENERATOR_FILTER_PARSER_HPP_INCLUDED

#include <catch2/internal/catch_optional.hpp>

#include <string>
#include <vector>

namespace Catch {

    struct GeneratorFilterParseResult {
        enum class Type { Ok, Error };
        Type type = Type::Error;
        std::vector<unsigned int> indices;
        std::string errorMessage;
    };

    /**
     * Parses generator index filter specs such as `1,3,6-10`.
     *
     * Indices must be monotonically increasing with no duplicates or
     * overlapping ranges.
     */
    GeneratorFilterParseResult parseGeneratorIndexSpec( std::string const& input );

} // end namespace Catch

#endif // CATCH_GENERATOR_FILTER_PARSER_HPP_INCLUDED
