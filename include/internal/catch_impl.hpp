/*
 *  Created by Phil on 5/8/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_IMPL_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_IMPL_HPP_INCLUDED

// Collect all the implementation files together here
// These are the equivalent of what would usually be cpp files

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif


#include "internal/catch_leak_detector.h"

#include "catch_test_spec.h"
#include "catch_test_case_tracker.h"

// Cpp files will be included in the single-header file here
// ~*~* CATCH_CPP_STITCH_PLACE *~*~

namespace Catch {
    LeakDetector leakDetector;

    // These are all here to avoid warnings about not having any out of line
    // virtual methods
    IResultCapture::~IResultCapture() {}
    ITestInvoker::~ITestInvoker() {}
    ITestCaseRegistry::~ITestCaseRegistry() {}
    IRegistryHub::~IRegistryHub() {}
    IMutableRegistryHub::~IMutableRegistryHub() {}
    IExceptionTranslator::~IExceptionTranslator() {}
    IExceptionTranslatorRegistry::~IExceptionTranslatorRegistry() {}
    IRunner::~IRunner() {}
    IConfig::~IConfig() {}

    void Config::dummy() {}
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_CATCH_IMPL_HPP_INCLUDED
