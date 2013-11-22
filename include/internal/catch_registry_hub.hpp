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

    class RegistryHub : public IRegistryHub, public IMutableRegistryHub {

        RegistryHub( RegistryHub const& );
        void operator=( RegistryHub const& );

    public: // IRegistryHub
        RegistryHub() {
        }
        virtual IReporterRegistry const& getReporterRegistry() const {
            return m_reporterRegistry;
        }
        virtual ITestCaseRegistry const& getTestCaseRegistry() const {
            return m_testCaseRegistry;
        }
        virtual IExceptionTranslatorRegistry& getExceptionTranslatorRegistry() {
            return m_exceptionTranslatorRegistry;
        }

    public: // IMutableRegistryHub
        virtual void registerReporter( std::string const& name, IReporterFactory* factory ) {
            m_reporterRegistry.registerReporter( name, factory );
        }
        virtual void registerTest( TestCase const& testInfo ) {
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

    INTERNAL_CATCH_INLINE IRegistryHub& getRegistryHub() {
        return *GlobalRegistryHub<RegistryHub>::instance();
    }
    INTERNAL_CATCH_INLINE IMutableRegistryHub& getMutableRegistryHub() {
        return *GlobalRegistryHub<RegistryHub>::instance();
    }
    INTERNAL_CATCH_INLINE void cleanUp() {
        delete GlobalRegistryHub<RegistryHub>::instance();
        GlobalRegistryHub<RegistryHub>::instance() = NULL;
        cleanUpContext();
    }
    INTERNAL_CATCH_INLINE std::string translateActiveException() {
        return getRegistryHub().getExceptionTranslatorRegistry().translateActiveException();
    }


} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REGISTRY_HUB_HPP_INCLUDED
