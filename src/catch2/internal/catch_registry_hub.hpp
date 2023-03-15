
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_REGISTRY_HUB_HPP_INCLUDED
#define CATCH_REGISTRY_HUB_HPP_INCLUDED

#include <catch2/internal/catch_unique_ptr.hpp>

#include <string>

namespace Catch {

    class TestCaseHandle;
    struct TestCaseInfo;
    class TestCaseRegistry;
    class ExceptionTranslatorRegistry;
    class IExceptionTranslator;
    class ReporterRegistry;
    class IReporterFactory;
    class TagAliasRegistry;
    class ITestInvoker;
    class EnumValuesRegistry;
    struct SourceLineInfo;

    class StartupExceptionRegistry;
    class EventListenerFactory;

    using IReporterFactoryPtr = Detail::unique_ptr<IReporterFactory>;

    class RegistryHub {
        struct RegistryHubImpl;
        Detail::unique_ptr<RegistryHubImpl> m_impl;
    public:
        RegistryHub();
        ~RegistryHub();

        ReporterRegistry const& getReporterRegistry() const;
        TestCaseRegistry const& getTestCaseRegistry() const;
        TagAliasRegistry const& getTagAliasRegistry() const;
        ExceptionTranslatorRegistry const& getExceptionTranslatorRegistry() const;
        StartupExceptionRegistry const& getStartupExceptionRegistry() const;

        void registerReporter( std::string const& name,
                               IReporterFactoryPtr factory );
        void registerListener( Detail::unique_ptr<EventListenerFactory> factory );
        void registerTest( Detail::unique_ptr<TestCaseInfo>&& testInfo,
                           Detail::unique_ptr<ITestInvoker>&& invoker );
        void registerTranslator( Detail::unique_ptr<IExceptionTranslator>&& translator );
        void registerTagAlias( std::string const& alias,
                               std::string const& tag,
                               SourceLineInfo const& lineInfo );
        void registerStartupException() noexcept;
        EnumValuesRegistry& getMutableEnumValuesRegistry();
    };

    RegistryHub const& getRegistryHub();
    RegistryHub& getMutableRegistryHub();
    void cleanUp();
    std::string translateActiveException();

}

#endif // CATCH_REGISTRY_HUB_HPP_INCLUDED
