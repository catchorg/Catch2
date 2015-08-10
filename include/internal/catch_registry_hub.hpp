/*
 *  Created by Phil on 5/8/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REGISTRY_HUB_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REGISTRY_HUB_HPP_INCLUDED

#include "catch_interfaces_registry_hub.h"

#include "catch_test_case_registry_impl.hpp"
#include "catch_reporter_registry.hpp"
#include "catch_exception_translator_registry.hpp"

namespace Catch {

    namespace {

        class RegistryHub : public IRegistryHub, public IMutableRegistryHub {

            RegistryHub( RegistryHub const& );
            void operator=( RegistryHub const& );

        public: // IRegistryHub
            RegistryHub() {
            }
            virtual IReporterRegistry const& getReporterRegistry() const CATCH_OVERRIDE {
                return m_reporterRegistry;
            }
            virtual ITestCaseRegistry const& getTestCaseRegistry() const CATCH_OVERRIDE {
                return m_testCaseRegistry;
            }
            virtual IExceptionTranslatorRegistry& getExceptionTranslatorRegistry() CATCH_OVERRIDE {
                return m_exceptionTranslatorRegistry;
            }

        public: // IMutableRegistryHub
            virtual void registerReporter( std::string const& name, Ptr<IReporterFactory> const& factory ) CATCH_OVERRIDE {
                m_reporterRegistry.registerReporter( name, factory );
            }
            virtual void registerListener( Ptr<IReporterFactory> const& factory ) CATCH_OVERRIDE {
                m_reporterRegistry.registerListener( factory );
            }
            virtual void registerTest( TestCase const& testInfo ) CATCH_OVERRIDE {
                m_testCaseRegistry.registerTest( testInfo );
            }
            virtual void registerTranslator( const IExceptionTranslator* translator ) CATCH_OVERRIDE {
                m_exceptionTranslatorRegistry.registerTranslator( translator );
            }

        private:
            TestRegistry m_testCaseRegistry;
            ReporterRegistry m_reporterRegistry;
            ExceptionTranslatorRegistry m_exceptionTranslatorRegistry;
        };

        // Single, global, instance
        inline RegistryHub*& getTheRegistryHub() {
            static RegistryHub* theRegistryHub = CATCH_NULL;
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
        getTheRegistryHub() = CATCH_NULL;
        cleanUpContext();
    }
    std::string translateActiveException() {
        return getRegistryHub().getExceptionTranslatorRegistry().translateActiveException();
    }


} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REGISTRY_HUB_HPP_INCLUDED
