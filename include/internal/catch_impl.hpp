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

#include "catch_runner.hpp"
#include "catch_registry_hub.hpp"
#include "catch_notimplemented_exception.hpp"
#include "catch_context_impl.hpp"
#include "catch_console_colour_impl.hpp"
#include "catch_generators_impl.hpp"
#include "catch_assertionresult.hpp"
#include "catch_expressionresult_builder.hpp"
#include "catch_test_case_info.hpp"
#include "catch_tags.hpp"
#include "catch_version.hpp"
#include "catch_text.hpp"
#include "catch_message.hpp"
#include "catch_legacy_reporter_adapter.hpp"

#include "../reporters/catch_reporter_basic.hpp"
#include "../reporters/catch_reporter_xml.hpp"
#include "../reporters/catch_reporter_junit.hpp"
#include "../reporters/catch_reporter_console.hpp"

namespace Catch {
    NonCopyable::~NonCopyable() {}
    IShared::~IShared() {}
    StreamBufBase::~StreamBufBase() throw() {}
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
    IStreamingReporter::~IStreamingReporter() {}
    AssertionStats::~AssertionStats() {}
    SectionStats::~SectionStats() {}
    TestCaseStats::~TestCaseStats() {}
    TestGroupStats::~TestGroupStats() {}
    TestRunStats::~TestRunStats() {}
    ThreadedSectionInfo::~ThreadedSectionInfo() {}
    TestGroupNode::~TestGroupNode() {}
    TestRunNode::~TestRunNode() {}

    BasicReporter::~BasicReporter() {}
    StreamingReporterBase::~StreamingReporterBase() {}
    ConsoleReporter::~ConsoleReporter() {}
    IRunner::~IRunner() {}
    IMutableContext::~IMutableContext() {}
    IConfig::~IConfig() {}
    XmlReporter::~XmlReporter() {}
    JunitReporter::~JunitReporter() {}
    TestRegistry::~TestRegistry() {}
    FreeFunctionTestCase::~FreeFunctionTestCase() {}
    IGeneratorInfo::~IGeneratorInfo() {}
    IGeneratorsForTest::~IGeneratorsForTest() {}
    TagParser::~TagParser() {}
    TagExtracter::~TagExtracter() {}
    TagExpressionParser::~TagExpressionParser() {}

    Matchers::Impl::StdString::Equals::~Equals() {}
    Matchers::Impl::StdString::Contains::~Contains() {}
    Matchers::Impl::StdString::StartsWith::~StartsWith() {}
    Matchers::Impl::StdString::EndsWith::~EndsWith() {}

    void Config::dummy() {}

    INTERNAL_CATCH_REGISTER_LEGACY_REPORTER( "basic", BasicReporter )
    INTERNAL_CATCH_REGISTER_LEGACY_REPORTER( "xml", XmlReporter )
    INTERNAL_CATCH_REGISTER_LEGACY_REPORTER( "junit", JunitReporter )

}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_CATCH_IMPL_HPP_INCLUDED
