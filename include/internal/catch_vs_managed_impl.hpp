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
    inline String^ convert_string_to_managed(const std::string& s)
    {
        String^ result = gcnew String(s.c_str());
        return result;
    }

}

#include "internal/catch_timer.hpp"
#include "internal/catch_reporter_registrars.hpp"
#include "reporters/catch_vs_reporter.hpp"
#include "catch_registry_hub.hpp"

//#define OLD (1)
#ifdef OLD
namespace Catch {

    class ExceptionRegistryHub : public IRegistryHub, public IMutableRegistryHub {

        ExceptionRegistryHub( ExceptionRegistryHub const& );
        void operator=( ExceptionRegistryHub const& );

    public: // IRegistryHub
        ExceptionRegistryHub() {
        }
        virtual IReporterRegistry const& getReporterRegistry() const {
            throw std::runtime_error("can't do this for Visual Studio tests!");
        }
        virtual ITestCaseRegistry const& getTestCaseRegistry() const {
            throw std::runtime_error("can't do this for Visual Studio tests!");
        }
        virtual IExceptionTranslatorRegistry& getExceptionTranslatorRegistry() {
            return m_exceptionTranslatorRegistry;
        }

    public: // IMutableRegistryHub
        virtual void registerReporter( std::string const&, IReporterFactory* ) {
            throw std::runtime_error("can't do this for Visual Studio tests!");
        }
        virtual void registerTest( TestCase const& ) {
            throw std::runtime_error("can't do this for Visual Studio tests!");
        }
        virtual void registerTranslator( const IExceptionTranslator* translator ) {
            m_exceptionTranslatorRegistry.registerTranslator( translator );
        }

    private:
        ExceptionTranslatorRegistry m_exceptionTranslatorRegistry;
    };

    template <typename T>
    struct GlobalRegistryHub
    {
        static T*& instance()
        {
            if( !theRegistryHub )
                theRegistryHub = new T();
            return theRegistryHub;
        }
        static T* theRegistryHub;
    };
    template <typename T>
    T* GlobalRegistryHub<T>::theRegistryHub = NULL;

    INTERNAL_CATCH_INLINE IMutableRegistryHub& getMutableRegistryHub() {
        return *GlobalRegistryHub<ExceptionRegistryHub>::instance();
    }
    INTERNAL_CATCH_INLINE std::string translateActiveException() {
        return GlobalRegistryHub<ExceptionRegistryHub>::instance()->getExceptionTranslatorRegistry().translateActiveException();
    }

}
#endif // OLD

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
    //CumulativeReporterBase::SectionNode::~SectionNode() {}
    //CumulativeReporterBase::~CumulativeReporterBase() {}

    //StreamingReporterBase::~StreamingReporterBase() {}
    //ConsoleReporter::~ConsoleReporter() {}
    inline IRunner::~IRunner() {}
    inline IMutableContext::~IMutableContext() {}
    inline IConfig::~IConfig() {}
    //XmlReporter::~XmlReporter() {}
    //JunitReporter::~JunitReporter() {}
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

    //INTERNAL_CATCH_REGISTER_LEGACY_REPORTER( "xml", XmlReporter )
}

#endif

#endif // TWOBLUECUBES_CATCH_VS_MANAGED_HPP_INCLUDED
