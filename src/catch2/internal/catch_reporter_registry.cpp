/*
 *  Created by Martin on 31/08/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include <catch2/internal/catch_reporter_registry.hpp>

#include <catch2/catch_reporter_registrars.hpp>
#include <catch2/reporters/catch_reporter_automake.hpp>
#include <catch2/reporters/catch_reporter_compact.hpp>
#include <catch2/reporters/catch_reporter_console.hpp>
#include <catch2/reporters/catch_reporter_junit.hpp>
#include <catch2/reporters/catch_reporter_sonarqube.hpp>
#include <catch2/reporters/catch_reporter_tap.hpp>
#include <catch2/reporters/catch_reporter_teamcity.hpp>
#include <catch2/reporters/catch_reporter_xml.hpp>

namespace Catch {

    ReporterRegistry::ReporterRegistry() {
        // Because it is impossible to move out of initializer list,
        // we have to add the elements manually
        m_factories["automake"] = std::make_unique<ReporterFactory<AutomakeReporter>>();
        m_factories["compact"] = std::make_unique<ReporterFactory<CompactReporter>>();
        m_factories["console"] = std::make_unique<ReporterFactory<ConsoleReporter>>();
        m_factories["junit"] = std::make_unique<ReporterFactory<JunitReporter>>();
        m_factories["sonarqube"] = std::make_unique<ReporterFactory<SonarQubeReporter>>();
        m_factories["tap"] = std::make_unique<ReporterFactory<TAPReporter>>();
        m_factories["teamcity"] = std::make_unique<ReporterFactory<TeamCityReporter>>();
        m_factories["xml"] = std::make_unique<ReporterFactory<XmlReporter>>();
    }

    ReporterRegistry::~ReporterRegistry() = default;


    IStreamingReporterPtr ReporterRegistry::create( std::string const& name, IConfig const* config ) const {
        auto it =  m_factories.find( name );
        if( it == m_factories.end() )
            return nullptr;
        return it->second->create( ReporterConfig( config ) );
    }

    void ReporterRegistry::registerReporter( std::string const& name, IReporterFactoryPtr factory ) {
        m_factories.emplace(name, std::move(factory));
    }
    void ReporterRegistry::registerListener( IReporterFactoryPtr factory ) {
        m_listeners.push_back( std::move(factory) );
    }

    IReporterRegistry::FactoryMap const& ReporterRegistry::getFactories() const {
        return m_factories;
    }
    IReporterRegistry::Listeners const& ReporterRegistry::getListeners() const {
        return m_listeners;
    }

}
