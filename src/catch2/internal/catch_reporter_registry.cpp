
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_reporter_registry.hpp>

#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <catch2/reporters/catch_reporter_automake.hpp>
#include <catch2/reporters/catch_reporter_compact.hpp>
#include <catch2/reporters/catch_reporter_console.hpp>
#include <catch2/reporters/catch_reporter_junit.hpp>
#include <catch2/reporters/catch_reporter_sonarqube.hpp>
#include <catch2/reporters/catch_reporter_tap.hpp>
#include <catch2/reporters/catch_reporter_teamcity.hpp>
#include <catch2/reporters/catch_reporter_xml.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>
#include <catch2/internal/catch_enforce.hpp>

namespace Catch {

    ReporterRegistry::ReporterRegistry() {
        // Because it is impossible to move out of initializer list,
        // we have to add the elements manually
        m_factories["Automake"] = Detail::make_unique<ReporterFactory<AutomakeReporter>>();
        m_factories["compact"] = Detail::make_unique<ReporterFactory<CompactReporter>>();
        m_factories["console"] = Detail::make_unique<ReporterFactory<ConsoleReporter>>();
        m_factories["JUnit"] = Detail::make_unique<ReporterFactory<JunitReporter>>();
        m_factories["SonarQube"] = Detail::make_unique<ReporterFactory<SonarQubeReporter>>();
        m_factories["TAP"] = Detail::make_unique<ReporterFactory<TAPReporter>>();
        m_factories["TeamCity"] = Detail::make_unique<ReporterFactory<TeamCityReporter>>();
        m_factories["XML"] = Detail::make_unique<ReporterFactory<XmlReporter>>();
    }

    ReporterRegistry::~ReporterRegistry() = default;


    IEventListenerPtr ReporterRegistry::create( std::string const& name, ReporterConfig&& config ) const {
        auto it =  m_factories.find( name );
        if( it == m_factories.end() )
            return nullptr;
        return it->second->create( CATCH_MOVE(config) );
    }

    void ReporterRegistry::registerReporter( std::string const& name, IReporterFactoryPtr factory ) {
        CATCH_ENFORCE( name.find( "::" ) == name.npos,
                       "'::' is not allowed in reporter name: '" + name + '\'' );
        auto ret = m_factories.emplace(name, CATCH_MOVE(factory));
        CATCH_ENFORCE( ret.second, "reporter using '" + name + "' as name was already registered" );
    }
    void ReporterRegistry::registerListener(
        Detail::unique_ptr<EventListenerFactory> factory ) {
        m_listeners.push_back( CATCH_MOVE(factory) );
    }

    IReporterRegistry::FactoryMap const& ReporterRegistry::getFactories() const {
        return m_factories;
    }
    IReporterRegistry::Listeners const& ReporterRegistry::getListeners() const {
        return m_listeners;
    }

}
