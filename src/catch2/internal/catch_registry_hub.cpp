
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_registry_hub.hpp>

#include <catch2/internal/catch_context.hpp>
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_test_case_registry_impl.hpp>
#include <catch2/internal/catch_reporter_registry.hpp>
#include <catch2/internal/catch_exception_translator_registry.hpp>
#include <catch2/internal/catch_tag_alias_registry.hpp>
#include <catch2/internal/catch_startup_exception_registry.hpp>
#include <catch2/internal/catch_singletons.hpp>
#include <catch2/internal/catch_enum_values_registry.hpp>
#include <catch2/catch_test_case_info.hpp>
#include <catch2/internal/catch_noncopyable.hpp>
#include <catch2/interfaces/catch_interfaces_reporter_factory.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>
#include <catch2/internal/catch_reporter_registry.hpp>

namespace Catch {

    struct RegistryHub::RegistryHubImpl {
        TestCaseRegistry testCaseRegistry;
        ReporterRegistry reporterRegistry;
        ExceptionTranslatorRegistry exceptionTranslatorRegistry;
        TagAliasRegistry tagAliasRegistry;
        StartupExceptionRegistry exceptionRegistry;
        EnumValuesRegistry enumValuesRegistry;
    };

    RegistryHub::RegistryHub():
        m_impl( Detail::make_unique<RegistryHubImpl>() ) {}
    RegistryHub::~RegistryHub() = default;

    ReporterRegistry const&
    RegistryHub::getReporterRegistry() const {
        return m_impl->reporterRegistry;
    }
    TestCaseRegistry const&
    RegistryHub::getTestCaseRegistry() const {
        return m_impl->testCaseRegistry;
    }
    ExceptionTranslatorRegistry const&
    RegistryHub::getExceptionTranslatorRegistry() const {
        return m_impl->exceptionTranslatorRegistry;
    }
    TagAliasRegistry const&
    RegistryHub::getTagAliasRegistry() const {
        return m_impl->tagAliasRegistry;
    }
    StartupExceptionRegistry const&
    RegistryHub::getStartupExceptionRegistry() const {
        return m_impl->exceptionRegistry;
    }
    void
    RegistryHub::registerReporter( std::string const& name,
                           IReporterFactoryPtr factory ) {
        m_impl->reporterRegistry.registerReporter( name, CATCH_MOVE( factory ) );
    }
    void RegistryHub::registerListener(
        Detail::unique_ptr<EventListenerFactory> factory ) {
        m_impl->reporterRegistry.registerListener( CATCH_MOVE( factory ) );
    }
    void RegistryHub::registerTest(
        Detail::unique_ptr<TestCaseInfo>&& testInfo,
                       Detail::unique_ptr<ITestInvoker>&& invoker ) {
        m_impl->testCaseRegistry.registerTest( CATCH_MOVE( testInfo ),
                                         CATCH_MOVE( invoker ) );
    }
    void RegistryHub::registerTranslator(
        Detail::unique_ptr<IExceptionTranslator>&& translator ) {
        m_impl->exceptionTranslatorRegistry.registerTranslator(
            CATCH_MOVE( translator ) );
    }
    void RegistryHub::registerTagAlias( std::string const& alias,
                           std::string const& tag,
                           SourceLineInfo const& lineInfo ) {
        m_impl->tagAliasRegistry.add( alias, tag, lineInfo );
    }
    void RegistryHub::registerStartupException() noexcept {
#if !defined( CATCH_CONFIG_DISABLE_EXCEPTIONS )
        m_impl->exceptionRegistry.add( std::current_exception() );
#else
        CATCH_INTERNAL_ERROR( "Attempted to register active exception under "
                              "CATCH_CONFIG_DISABLE_EXCEPTIONS!" );
#endif
    }
    EnumValuesRegistry& RegistryHub::getMutableEnumValuesRegistry() {
        return m_impl->enumValuesRegistry;
    }

    using RegistryHubSingleton = Singleton<RegistryHub>;

    RegistryHub const& getRegistryHub() {
        return RegistryHubSingleton::get();
    }
    RegistryHub& getMutableRegistryHub() {
        return RegistryHubSingleton::getMutable();
    }
    void cleanUp() {
        cleanupSingletons();
        cleanUpContext();
    }
    std::string translateActiveException() {
        return getRegistryHub().getExceptionTranslatorRegistry().translateActiveException();
    }


} // end namespace Catch
