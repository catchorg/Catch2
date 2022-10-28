
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_REPORTER_REGISTRY_HPP_INCLUDED
#define CATCH_REPORTER_REGISTRY_HPP_INCLUDED

#include <catch2/interfaces/catch_interfaces_reporter.hpp>
#include <catch2/interfaces/catch_interfaces_reporter_registry.hpp>

#include <map>

namespace Catch {

    class ReporterRegistry : public IReporterRegistry {
    public:

        ReporterRegistry();
        ~ReporterRegistry() override; // = default, out of line to allow fwd decl

        IEventListenerPtr create( std::string const& name, ReporterConfig&& config ) const override;

        void registerReporter( std::string const& name, IReporterFactoryPtr factory );
        void registerListener( Detail::unique_ptr<EventListenerFactory> factory );

        FactoryMap const& getFactories() const override;
        Listeners const& getListeners() const override;

    private:
        FactoryMap m_factories;
        Listeners m_listeners;
    };
}

#endif // CATCH_REPORTER_REGISTRY_HPP_INCLUDED
