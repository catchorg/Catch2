/*
 *  Created by Martin on 31/08/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "catch_reporter_registry.h"

namespace Catch {

    ReporterRegistry::~ReporterRegistry() = default;

    IStreamingReporterPtr ReporterRegistry::create( std::string const& name, IConfigPtr const& config ) const {
        auto it =  m_factories.find( name );
        if( it == m_factories.end() )
            return nullptr;
        return it->second->create( ReporterConfig( config ) );
    }

    void ReporterRegistry::registerReporter( std::string const& name, IReporterFactoryPtr const& factory ) {
        m_factories.emplace(name, factory);
    }
    void ReporterRegistry::registerListener( IReporterFactoryPtr const& factory ) {
        m_listeners.push_back( factory );
    }

    IReporterRegistry::FactoryMap const& ReporterRegistry::getFactories() const {
        return m_factories;
    }
    IReporterRegistry::Listeners const& ReporterRegistry::getListeners() const {
        return m_listeners;
    }

}
