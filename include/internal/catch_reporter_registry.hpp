/*
 *  Created by Phil on 29/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_REGISTRY_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_REGISTRY_HPP_INCLUDED

#include "catch_interfaces_reporter.h"

#include <map>

namespace Catch {

    class ReporterRegistry : public IReporterRegistry {

    public:

        virtual ~ReporterRegistry() CATCH_OVERRIDE {}

        virtual IStreamingReporter* create( std::string const& name, Ptr<IConfig const> const& config ) const CATCH_OVERRIDE {
            FactoryMap::const_iterator it =  m_factories.find( name );
            if( it == m_factories.end() )
                return CATCH_NULL;
            return it->second->create( ReporterConfig( config ) );
        }

        void registerReporter( std::string const& name, Ptr<IReporterFactory> const& factory ) {
            m_factories.insert( std::make_pair( name, factory ) );
        }
        void registerListener( Ptr<IReporterFactory> const& factory ) {
            m_listeners.push_back( factory );
        }

        virtual FactoryMap const& getFactories() const CATCH_OVERRIDE {
            return m_factories;
        }
        virtual Listeners const& getListeners() const CATCH_OVERRIDE {
            return m_listeners;
        }

    private:
        FactoryMap m_factories;
        Listeners m_listeners;
    };
}

#endif // TWOBLUECUBES_CATCH_REPORTER_REGISTRY_HPP_INCLUDED
