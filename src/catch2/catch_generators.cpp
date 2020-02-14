/*
 *  Created by Phil Nash on 15/6/2018.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <catch2/catch_generators.hpp>
#include <catch2/catch_enforce.h>
#include <catch2/catch_generator_exception.hpp>
#include <catch2/catch_interfaces_capture.h>

#include <limits>
#include <set>

namespace Catch {

IGeneratorTracker::~IGeneratorTracker() {}

namespace Generators {

namespace Detail {

    [[noreturn]]
    void throw_generator_exception(char const* msg) {
        Catch::throw_exception(GeneratorException{ msg });
    }
} // end namespace Detail

    GeneratorUntypedBase::~GeneratorUntypedBase() {}

    auto acquireGeneratorTracker( SourceLineInfo const& lineInfo ) -> IGeneratorTracker& {
        return getResultCapture().acquireGeneratorTracker( lineInfo );
    }

} // namespace Generators
} // namespace Catch
