/*
 *  Created by Phil on 5/8/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "catch_interfaces_registry_hub.h"

#include "catch_test_case_registry_impl.hpp"
#include "catch_reporter_registry.hpp"
#include "catch_exception_translator_registry.hpp"

namespace Catch {

    namespace {
        
        class RegistryHub : public IRegistryHub, public IMutableRegistryHub {

            RegistryHub( const RegistryHub& );
            void operator=( const RegistryHub& );

        public: // IRegistryHub
            RegistryHub() {
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

        public: // IMutableRegistryHub
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

        // Single, global, instance
        inline RegistryHub*& getTheRegistryHub() {
            static RegistryHub* theRegistryHub = NULL;
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
        getTheRegistryHub() = NULL;
        cleanUpContext();
    }

} // end namespace Catch
