
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/internal/catch_generator_info.hpp>

namespace Catch {

    GeneratorInfo::GeneratorInfo( StringRef _name,
                                  StringRef _arguments,
                                  SourceLineInfo const& _lineInfo,
                                  StringRef _currentElement ):
        name( _name ),
        arguments( _arguments ),
        lineInfo( _lineInfo ),
        currentElement( _currentElement ) {}

} // end namespace Catch
