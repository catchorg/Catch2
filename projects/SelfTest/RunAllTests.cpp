/*
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"
#include "internal/catch_text.h"
#include "internal/catch_console_colour.hpp"

namespace AllTestsRunner {

    // VS2010
    // mstest /TestContainer:Debug\ManagedTestCatch.dll /category:"all"
    //
    // VS2012 managed
    // vstest.console.exe /Logger:Trx Debug\ManagedTestCatch.dll /TestCaseFilter:"TestCategory=all"
    //
    // VS2012 native
    // vstest.console.exe /Logger:Trx Debug\NativeTestCatch.dll /TestCaseFilter:"Owner=all"
    #if defined(INTERNAL_CATCH_VS_MANAGED) || defined(INTERNAL_CATCH_VS_NATIVE)
        CATCH_MAP_CATEGORY_TO_TAG(all, "~[vs]");

        CATCH_CONFIG_SHOW_SUCCESS(true)
        CATCH_CONFIG_WARN_MISSING_ASSERTIONS(true)
        CATCH_MAP_CATEGORY_TO_TAG(allSucceeding, "~[vs]");

        CATCH_CONFIG_SHOW_SUCCESS(true)
        CATCH_CONFIG_WARN_MISSING_ASSERTIONS(true)
        CATCH_CONFIG_ABORT_AFTER(4)
        CATCH_INTERNAL_CONFIG_ADD_TEST("Some simple comparisons between doubles")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Approximate comparisons with different epsilons")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Approximate comparisons with floats")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Approximate comparisons with ints")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Approximate comparisons with mixed numeric types")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Use a custom approx")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Approximate PI")
        CATCH_INTERNAL_CONFIG_ADD_TEST("A METHOD_AS_TEST_CASE based test run that succeeds")
        CATCH_INTERNAL_CONFIG_ADD_TEST("A METHOD_AS_TEST_CASE based test run that fails")
        CATCH_INTERNAL_CONFIG_ADD_TEST("A TEST_CASE_METHOD based test run that succeeds")
        CATCH_INTERNAL_CONFIG_ADD_TEST("A TEST_CASE_METHOD based test run that fails")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Equality checks that should succeed")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Equality checks that should fail]")
        INTERNAL_CATCH_MAP_CATEGORY_TO_LIST(allSucceedingAborting);

        CATCH_INTERNAL_CONFIG_ADD_TEST("Output from all sections is reported")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Standard output from all sections is reported")
        INTERNAL_CATCH_MAP_CATEGORY_TO_LIST(OutputFromAllSectionsIsReported);
#endif

}
