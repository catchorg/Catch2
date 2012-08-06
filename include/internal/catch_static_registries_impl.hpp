/*
 *  Created by Phil on 5/8/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "catch_interfaces_static_registries.h"

#include "catch_test_case_registry_impl.hpp"
#include "catch_reporter_registry.hpp"
#include "catch_exception_translator_registry.hpp"

namespace Catch {

    namespace {
        
        class StaticRegistries : public IStatics, public IStaticRegistries {

            StaticRegistries( const StaticRegistries& );
            void operator=( const StaticRegistries& );

        public: // IStatics
            StaticRegistries() {
            }
            virtual const IReporterRegistry& getReporterRegistry() const {
                return m_reporterRegistry;
            }
            virtual const ITestCaseRegistry& getTestCaseRegistry() const {
                return m_testCaseRegistry;
            }
            virtual IExceptionTranslatorRegistry& getExceptionTranslatorRegistry() {
                return m_exceptionTranslatorRegistry;
            }

        public: // IStaticRegistries
            virtual void registerReporter( const std::string& name, IReporterFactory* factory ) {
                m_reporterRegistry.registerReporter( name, factory );
            }
            virtual void registerTest( const TestCaseInfo& testInfo ) {
                m_testCaseRegistry.registerTest( testInfo );
            }
            virtual void registerTranslator( const IExceptionTranslator* translator ) {
                m_exceptionTranslatorRegistry.registerTranslator( translator );
            }

        private:
            TestRegistry m_testCaseRegistry;
            ReporterRegistry m_reporterRegistry;
            ExceptionTranslatorRegistry m_exceptionTranslatorRegistry;
        };

        inline StaticRegistries*& getTheStaticRegistries() {
            static StaticRegistries* registries = NULL;
            if( !registries )
                registries = new StaticRegistries();
            return registries;
        }
    }

    IStatics& getStatics() {
        return *getTheStaticRegistries();
    }
    IStaticRegistries& getStaticRegistries() {
        return *getTheStaticRegistries();
    }
    void cleanUp() {
        delete getTheStaticRegistries();
        getTheStaticRegistries() = NULL;
        Context::cleanUp();
    }

} // end namespace Catch
