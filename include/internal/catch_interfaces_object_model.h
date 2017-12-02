// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef TWOBLUECUBES_CATCH_INTERFACES_OBJECT_MODEL_H_INCLUDED
#define TWOBLUECUBES_CATCH_INTERFACES_OBJECT_MODEL_H_INCLUDED

#include "catch_option.hpp"
#include "catch_assertionresult.h"

#include <vector>

namespace Catch {

    struct TestCaseInfo;
    struct SectionInfo;
    struct AssertionStats;

    struct IObjectModel {
        virtual auto getCurrentTestCaseInfo() -> Option<TestCaseInfo> const & = 0;
        virtual auto getSectionInfos() -> std::vector<SectionInfo> const& = 0;
        virtual auto getLastAssertionStats() -> Option<AssertionStats> const& = 0;

        virtual ~IObjectModel();
    };

    auto getObjectModel() -> IObjectModel *;

} // namespace Catch

#endif //TWOBLUECUBES_CATCH_INTERFACES_OBJECT_MODEL_H_INCLUDED
