/*
 *  Created by Phil on 5/8/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_interfaces_registry_hub.h"

#include "catch_context.h"
#include "catch_test_case_registry_impl.h"
#include "catch_reporter_registry.h"
#include "catch_exception_translator_registry.h"
#include "catch_tag_alias_registry.h"
#include "catch_startup_exception_registry.h"

namespace Catch {

    namespace {

        class RegistryHub : public IRegistryHub, public IMutableRegistryHub,
                            private NonCopyable {

        public: // IRegistryHub
            RegistryHub() = default;
            IReporterRegistry const& getReporterRegistry() const override {
                return m_reporterRegistry;
            }
            ITestCaseRegistry const& getTestCaseRegistry() const override {
                return m_testCaseRegistry;
            }
            IExceptionTranslatorRegistry& getExceptionTranslatorRegistry() override {
                return m_exceptionTranslatorRegistry;
            }
            ITagAliasRegistry const& getTagAliasRegistry() const override {
                return m_tagAliasRegistry;
            }
            StartupExceptionRegistry const& getStartupExceptionRegistry() const override {
                return m_exceptionRegistry;
            }

        public: // IMutableRegistryHub
            void registerReporter( std::string const& name, IReporterFactoryPtr const& factory ) override {
                m_reporterRegistry.registerReporter( name, factory );
            }
            void registerListener( IReporterFactoryPtr const& factory ) override {
                m_reporterRegistry.registerListener( factory );
            }
            void registerTest( TestCase const& testInfo ) override {
                m_testCaseRegistry.registerTest( testInfo );
            }
            void registerTranslator( const IExceptionTranslator* translator ) override {
                m_exceptionTranslatorRegistry.registerTranslator( translator );
            }
            void registerTagAlias( std::string const& alias, std::string const& tag, SourceLineInfo const& lineInfo ) override {
                m_tagAliasRegistry.add( alias, tag, lineInfo );
            }
            void registerStartupException() noexcept override {
                m_exceptionRegistry.add(std::current_exception());
            }

        private:
            TestRegistry m_testCaseRegistry;
            ReporterRegistry m_reporterRegistry;
            ExceptionTranslatorRegistry m_exceptionTranslatorRegistry;
            TagAliasRegistry m_tagAliasRegistry;
            StartupExceptionRegistry m_exceptionRegistry;
        };

        // Single, global, instance
        RegistryHub*& getTheRegistryHub() {
            static RegistryHub* theRegistryHub = nullptr;
            if( !theRegistryHub )
                theRegistryHub = new RegistryHub();
            return theRegistryHub;
        }
    }

    IRegistryHub& getRegistryHub() {
        return *getTheRegistryHub();
    }
    IMutableRegistryHub& getMutableRegistryHub() {
        return *getTheRegistryHub();
    }
    void cleanUp() {
        delete getTheRegistryHub();
        getTheRegistryHub() = nullptr;
        cleanUpContext();
        ReusableStringStream::cleanup();
    }
    std::string translateActiveException() {
        return getRegistryHub().getExceptionTranslatorRegistry().translateActiveException();
    }


} // end namespace Catch
