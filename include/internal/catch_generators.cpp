/*
 *  Created by Phil Nash on 15/6/2018.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_generators.hpp"
#include "catch_random_number_generator.h"
#include "catch_interfaces_capture.h"

#include <limits>
#include <set>

namespace Catch {

IGeneratorTracker::~IGeneratorTracker() {}

const char* GeneratorException::what() const noexcept {
    return m_msg;
}

namespace Generators {

    GeneratorUntypedBase::~GeneratorUntypedBase() {}

    auto acquireGeneratorTracker( StringRef generatorName, SourceLineInfo const& lineInfo ) -> IGeneratorTracker& {
        return getResultCapture().acquireGeneratorTracker( generatorName, lineInfo );
    }

} // namespace Generators
} // namespace Catch
