/*
 *  Created by Phil on 5/8/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// Collect all the implementation files together here
// These are the equivalent of what would usually be cpp files

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif

#include "catch_runner.hpp"
#include "catch_registry_hub.hpp"
#include "catch_notimplemented_exception.hpp"
#include "catch_context_impl.hpp"
#include "catch_console_colour_impl.hpp"
#include "catch_generators_impl.hpp"
#include "catch_resultinfo.hpp"
#include "catch_resultinfo_builder.hpp"
#include "catch_test_case_info.hpp"

#include "../reporters/catch_reporter_basic.hpp"
#include "../reporters/catch_reporter_xml.hpp"
#include "../reporters/catch_reporter_junit.hpp"

namespace Catch {
    NonCopyable::~NonCopyable() {}
    IShared::~IShared() {}
    StreamBufBase::~StreamBufBase() {}
    IContext::~IContext() {}
    IResultCapture::~IResultCapture() {}
    ITestCase::~ITestCase() {}
    ITestCaseRegistry::~ITestCaseRegistry() {}
    IRegistryHub::~IRegistryHub() {}
    IMutableRegistryHub::~IMutableRegistryHub() {}
    IExceptionTranslator::~IExceptionTranslator() {}
    IExceptionTranslatorRegistry::~IExceptionTranslatorRegistry() {}
    IReporter::~IReporter() {}
    IReporterFactory::~IReporterFactory() {}
    IReporterRegistry::~IReporterRegistry() {}
    BasicReporter::~BasicReporter() {}
    IRunner::~IRunner() {}
    IMutableContext::~IMutableContext() {}
    IConfig::~IConfig() {}
    XmlReporter::~XmlReporter() {}
    JunitReporter::~JunitReporter() {}
    TestRegistry::~TestRegistry() {}
    FreeFunctionTestCase::~FreeFunctionTestCase() {}
    IGeneratorInfo::~IGeneratorInfo() {}
    IGeneratorsForTest::~IGeneratorsForTest() {}

    void Config::dummy() {}

    INTERNAL_CATCH_REGISTER_REPORTER( "basic", BasicReporter )
    INTERNAL_CATCH_REGISTER_REPORTER( "xml", XmlReporter )
    INTERNAL_CATCH_REGISTER_REPORTER( "junit", JunitReporter )

}

#ifdef __clang__
#pragma clang diagnostic pop
#endif
