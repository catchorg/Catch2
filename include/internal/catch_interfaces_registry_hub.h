/*
 *  Created by Phil on 5/8/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_STATIC_REGISTRIES_H_INCLUDED
#define TWOBLUECUBES_CATCH_STATIC_REGISTRIES_H_INCLUDED

#include "catch_interfaces_reporter.h"
#include "catch_interfaces_config.h"

#include <memory>
#include <vector>
#include <stdlib.h>

namespace Catch {

    class TestCaseInfo;
    struct IResultCapture;
    struct ITestCaseRegistry;
    struct IExceptionTranslatorRegistry;
    struct IExceptionTranslator;

    struct IRegistryHub {
        virtual ~IRegistryHub(){}

        virtual const IReporterRegistry& getReporterRegistry() const = 0;
        virtual const ITestCaseRegistry& getTestCaseRegistry() const = 0;
        virtual IExceptionTranslatorRegistry& getExceptionTranslatorRegistry() = 0;
    };

    struct IMutableRegistryHub {
        virtual ~IMutableRegistryHub(){}
        virtual void registerReporter( const std::string& name, IReporterFactory* factory ) = 0;
        virtual void registerTest( const TestCaseInfo& testInfo ) = 0;
        virtual void registerTranslator( const IExceptionTranslator* translator ) = 0;
    };

    IRegistryHub& getRegistryHub();
    IMutableRegistryHub& getMutableRegistryHub();
    void cleanUp();
}

#endif // TWOBLUECUBES_CATCH_STATIC_REGISTRIES_H_INCLUDED
