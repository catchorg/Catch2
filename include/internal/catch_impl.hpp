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
#include "catch_test_spec.hpp"
#include "catch_version.hpp"
#include "catch_message.hpp"
#include "catch_legacy_reporter_adapter.hpp"
#include "catch_timer.hpp"
#include "catch_common.hpp"
#include "catch_section.hpp"
#include "catch_debugger.hpp"

#include "../reporters/catch_reporter_xml.hpp"
#include "../reporters/catch_reporter_junit.hpp"
#include "../reporters/catch_reporter_console.hpp"

namespace Catch {
    INTERNAL_CATCH_INLINE NonCopyable::~NonCopyable() {}
    INTERNAL_CATCH_INLINE IShared::~IShared() {}
    INTERNAL_CATCH_INLINE StreamBufBase::~StreamBufBase() throw() {}
    INTERNAL_CATCH_INLINE IContext::~IContext() {}
    INTERNAL_CATCH_INLINE IResultCapture::~IResultCapture() {}
    INTERNAL_CATCH_INLINE ITestCase::~ITestCase() {}
    INTERNAL_CATCH_INLINE ITestCaseRegistry::~ITestCaseRegistry() {}
    INTERNAL_CATCH_INLINE IRegistryHub::~IRegistryHub() {}
    INTERNAL_CATCH_INLINE IMutableRegistryHub::~IMutableRegistryHub() {}
    INTERNAL_CATCH_INLINE IExceptionTranslator::~IExceptionTranslator() {}
    INTERNAL_CATCH_INLINE IExceptionTranslatorRegistry::~IExceptionTranslatorRegistry() {}
    INTERNAL_CATCH_INLINE IReporter::~IReporter() {}
    INTERNAL_CATCH_INLINE IReporterFactory::~IReporterFactory() {}
    INTERNAL_CATCH_INLINE IReporterRegistry::~IReporterRegistry() {}
    INTERNAL_CATCH_INLINE IStreamingReporter::~IStreamingReporter() {}
    INTERNAL_CATCH_INLINE AssertionStats::~AssertionStats() {}
    INTERNAL_CATCH_INLINE SectionStats::~SectionStats() {}
    INTERNAL_CATCH_INLINE TestCaseStats::~TestCaseStats() {}
    INTERNAL_CATCH_INLINE TestGroupStats::~TestGroupStats() {}
    INTERNAL_CATCH_INLINE TestRunStats::~TestRunStats() {}
    INTERNAL_CATCH_INLINE CumulativeReporterBase::SectionNode::~SectionNode() {}
    INTERNAL_CATCH_INLINE CumulativeReporterBase::~CumulativeReporterBase() {}

    INTERNAL_CATCH_INLINE StreamingReporterBase::~StreamingReporterBase() {}
    INTERNAL_CATCH_INLINE ConsoleReporter::~ConsoleReporter() {}
    INTERNAL_CATCH_INLINE IRunner::~IRunner() {}
    INTERNAL_CATCH_INLINE IMutableContext::~IMutableContext() {}
    INTERNAL_CATCH_INLINE IConfig::~IConfig() {}
    INTERNAL_CATCH_INLINE XmlReporter::~XmlReporter() {}
    INTERNAL_CATCH_INLINE JunitReporter::~JunitReporter() {}
    INTERNAL_CATCH_INLINE TestRegistry::~TestRegistry() {}
    INTERNAL_CATCH_INLINE FreeFunctionTestCase::~FreeFunctionTestCase() {}
    INTERNAL_CATCH_INLINE IGeneratorInfo::~IGeneratorInfo() {}
    INTERNAL_CATCH_INLINE IGeneratorsForTest::~IGeneratorsForTest() {}

    INTERNAL_CATCH_INLINE Matchers::Impl::StdString::Equals::~Equals() {}
    INTERNAL_CATCH_INLINE Matchers::Impl::StdString::Contains::~Contains() {}
    INTERNAL_CATCH_INLINE Matchers::Impl::StdString::StartsWith::~StartsWith() {}
    INTERNAL_CATCH_INLINE Matchers::Impl::StdString::EndsWith::~EndsWith() {}

    INTERNAL_CATCH_INLINE void Config::dummy() {}

    INTERNAL_CATCH_REGISTER_LEGACY_REPORTER( "xml", XmlReporter )
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_CATCH_IMPL_HPP_INCLUDED
