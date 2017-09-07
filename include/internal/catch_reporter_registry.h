/*
 *  Created by Phil on 29/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_REGISTRY_H_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_REGISTRY_H_INCLUDED

#include "catch_interfaces_reporter.h"

#include <map>

namespace Catch {

    class ReporterRegistry : public IReporterRegistry {

    public:

        ~ReporterRegistry() override;

        IStreamingReporterPtr create( std::string const& name, IConfigPtr const& config ) const override;

        void registerReporter( std::string const& name, IReporterFactoryPtr const& factory );
        void registerListener( IReporterFactoryPtr const& factory );

        FactoryMap const& getFactories() const override;
        Listeners const& getListeners() const override;

    private:
        FactoryMap m_factories;
        Listeners m_listeners;
    };
}

#endif // TWOBLUECUBES_CATCH_REPORTER_REGISTRY_H_INCLUDED
