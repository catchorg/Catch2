/*
 *  Created by Phil on 5/8/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_INTERFACES_REGISTRY_HUB_H_INCLUDED
#define TWOBLUECUBES_CATCH_INTERFACES_REGISTRY_HUB_H_INCLUDED

#include "catch_common.h"

#include <string>
#include <memory>

namespace Catch {

    class TestCase;
    struct ITestCaseRegistry;
    struct IExceptionTranslatorRegistry;
    struct IExceptionTranslator;
    struct IReporterRegistry;
    struct IReporterFactory;
    struct ITagAliasRegistry;
    class StartupExceptionRegistry;

    using IReporterFactoryPtr = std::shared_ptr<IReporterFactory>;

    struct IRegistryHub {
        virtual ~IRegistryHub();

        virtual IReporterRegistry const& getReporterRegistry() const = 0;
        virtual ITestCaseRegistry const& getTestCaseRegistry() const = 0;
        virtual ITagAliasRegistry const& getTagAliasRegistry() const = 0;

        virtual IExceptionTranslatorRegistry const& getExceptionTranslatorRegistry() const = 0;


        virtual StartupExceptionRegistry const& getStartupExceptionRegistry() const = 0;
    };

    struct IMutableRegistryHub {
        virtual ~IMutableRegistryHub();
        virtual void registerReporter( std::string const& name, IReporterFactoryPtr const& factory ) = 0;
        virtual void registerListener( IReporterFactoryPtr const& factory ) = 0;
        virtual void registerTest( TestCase const& testInfo ) = 0;
        virtual void registerTranslator( const IExceptionTranslator* translator ) = 0;
        virtual void registerTagAlias( std::string const& alias, std::string const& tag, SourceLineInfo const& lineInfo ) = 0;
        virtual void registerStartupException() noexcept = 0;
    };

    IRegistryHub const& getRegistryHub();
    IMutableRegistryHub& getMutableRegistryHub();
    void cleanUp();
    std::string translateActiveException();

}

#endif // TWOBLUECUBES_CATCH_INTERFACES_REGISTRY_HUB_H_INCLUDED
