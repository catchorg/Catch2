/*
 *  Created by Malcolm on 10/11/2013.
 *  Copyright 2013 Malcolm Noyes
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef TWOBLUECUBES_CATCH_VS_MANAGED_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_VS_MANAGED_HPP_INCLUDED

#ifdef INTERNAL_CATCH_VS_MANAGED

#include <windows.h>
using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;

namespace Catch {
    inline String^ convert_string_for_assert(const std::string& s)
    {
        String^ result = gcnew String(s.c_str());
        return result;
    }

}

#include "internal/catch_reporter_registrars.hpp"
#include "reporters/catch_vs_reporter.hpp"
#include "catch_registry_hub.hpp"
#include "internal/catch_timer.hpp"
#include "internal/catch_console_colour_impl.hpp"
#include "catch_runner.hpp"

namespace Catch {
    inline NonCopyable::~NonCopyable() {}
    inline IShared::~IShared() {}
    inline StreamBufBase::~StreamBufBase() throw() {}
    inline IContext::~IContext() {}
    inline IResultCapture::~IResultCapture() {}
    inline ITestCase::~ITestCase() {}
    inline ITestCaseRegistry::~ITestCaseRegistry() {}
    inline IRegistryHub::~IRegistryHub() {}
    inline IMutableRegistryHub::~IMutableRegistryHub() {}
    inline IExceptionTranslator::~IExceptionTranslator() {}
    inline IExceptionTranslatorRegistry::~IExceptionTranslatorRegistry() {}
    inline IReporter::~IReporter() {}
    inline IReporterFactory::~IReporterFactory() {}
    inline IReporterRegistry::~IReporterRegistry() {}
    inline IStreamingReporter::~IStreamingReporter() {}
    inline AssertionStats::~AssertionStats() {}
    inline SectionStats::~SectionStats() {}
    inline TestCaseStats::~TestCaseStats() {}
    inline TestGroupStats::~TestGroupStats() {}
    inline TestRunStats::~TestRunStats() {}

    inline IRunner::~IRunner() {}
    inline IMutableContext::~IMutableContext() {}
    inline IConfig::~IConfig() {}
    inline TestRegistry::~TestRegistry() {}
    inline FreeFunctionTestCase::~FreeFunctionTestCase() {}
    inline IGeneratorInfo::~IGeneratorInfo() {}
    inline IGeneratorsForTest::~IGeneratorsForTest() {}
    inline TagParser::~TagParser() {}
    inline TagExtracter::~TagExtracter() {}
    inline TagExpressionParser::~TagExpressionParser() {}

    inline Matchers::Impl::StdString::Equals::~Equals() {}
    inline Matchers::Impl::StdString::Contains::~Contains() {}
    inline Matchers::Impl::StdString::StartsWith::~StartsWith() {}
    inline Matchers::Impl::StdString::EndsWith::~EndsWith() {}

    inline void Config::dummy() {}
}

#endif

#endif // TWOBLUECUBES_CATCH_VS_MANAGED_HPP_INCLUDED
