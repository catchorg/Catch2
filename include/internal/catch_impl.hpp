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

#include "../catch_session.hpp"
#include "catch_registry_hub.hpp"
#include "catch_notimplemented_exception.hpp"
#include "catch_context_impl.hpp"
#include "catch_console_colour_impl.hpp"
#include "catch_generators_impl.hpp"
#include "catch_assertionresult.hpp"
#include "catch_test_case_info.hpp"
#include "catch_test_spec.hpp"
#include "catch_version.hpp"
#include "catch_message.hpp"
#include "catch_legacy_reporter_adapter.hpp"
#include "catch_timer.hpp"
#include "catch_common.hpp"
#include "catch_section.hpp"
#include "catch_debugger.hpp"
#include "catch_tostring.hpp"
#include "catch_result_builder.hpp"
#include "catch_tag_alias_registry.hpp"
#include "catch_test_case_tracker.hpp"
#include "catch_matchers_string.hpp"

#include "../reporters/catch_reporter_multi.hpp"
#include "../reporters/catch_reporter_xml.hpp"
#include "../reporters/catch_reporter_junit.hpp"
#include "../reporters/catch_reporter_console.hpp"
#include "../reporters/catch_reporter_compact.hpp"

namespace Catch {
    // These are all here to avoid warnings about not having any out of line
    // virtual methods
    NonCopyable::~NonCopyable() {}
    IShared::~IShared() {}
    IStream::~IStream() CATCH_NOEXCEPT {}
    FileStream::~FileStream() CATCH_NOEXCEPT {}
    CoutStream::~CoutStream() CATCH_NOEXCEPT {}
    DebugOutStream::~DebugOutStream() CATCH_NOEXCEPT {}
    StreamBufBase::~StreamBufBase() CATCH_NOEXCEPT {}
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
    CumulativeReporterBase::SectionNode::~SectionNode() {}
    CumulativeReporterBase::~CumulativeReporterBase() {}

    StreamingReporterBase::~StreamingReporterBase() {}
    ConsoleReporter::~ConsoleReporter() {}
    CompactReporter::~CompactReporter() {}
    IRunner::~IRunner() {}
    IMutableContext::~IMutableContext() {}
    IConfig::~IConfig() {}
    XmlReporter::~XmlReporter() {}
    JunitReporter::~JunitReporter() {}
    TestRegistry::~TestRegistry() {}
    FreeFunctionTestCase::~FreeFunctionTestCase() {}
    IGeneratorInfo::~IGeneratorInfo() {}
    IGeneratorsForTest::~IGeneratorsForTest() {}
    WildcardPattern::~WildcardPattern() {}
    TestSpec::Pattern::~Pattern() {}
    TestSpec::NamePattern::~NamePattern() {}
    TestSpec::TagPattern::~TagPattern() {}
    TestSpec::ExcludedPattern::~ExcludedPattern() {}
    Matchers::Impl::MatcherUntypedBase::~MatcherUntypedBase() {}

    void Config::dummy() {}

    namespace TestCaseTracking {
        ITracker::~ITracker() {}
        TrackerBase::~TrackerBase() {}
        SectionTracker::~SectionTracker() {}
        IndexTracker::~IndexTracker() {}
    }
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_CATCH_IMPL_HPP_INCLUDED
