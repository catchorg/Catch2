/*
 *  Created by Malcolm on 10/11/2013.
 *  Copyright 2013 Malcolm Noyes
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef TWOBLUECUBES_CATCH_VS_NATIVE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_VS_NATIVE_HPP_INCLUDED

#ifdef INTERNAL_CATCH_VS_NATIVE

#include "CppUnitTest.h"
using Microsoft::VisualStudio::CppUnitTestFramework::Logger;
using Microsoft::VisualStudio::CppUnitTestFramework::Assert;
using Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo;

#define INTERNAL_CATCH_INLINE inline

#include <cvt/wstring>
#include <codecvt>

#include "internal/catch_timer.hpp"
#include "internal/catch_vs_test_registry.hpp"
#include "reporters/catch_vs_reporter.hpp"

#include "internal/catch_exception_translator_registry.hpp"

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
        static T& instance()
        {
            if( !theRegistryHub )
                theRegistryHub = new T();
            return *theRegistryHub;
        }
        static T* theRegistryHub;
    };
    template <typename T>
    T* GlobalRegistryHub<T>::theRegistryHub = NULL;

    INTERNAL_CATCH_INLINE IMutableRegistryHub& getMutableRegistryHub() {
        return GlobalRegistryHub<ExceptionRegistryHub>::instance();
    }
    INTERNAL_CATCH_INLINE std::string translateActiveException() {
        return GlobalRegistryHub<ExceptionRegistryHub>::instance().getExceptionTranslatorRegistry().translateActiveException();
    }

}

#endif // INTERNAL_CATCH_VS_NATIVE

#endif // TWOBLUECUBES_CATCH_VS_NATIVE_HPP_INCLUDED
