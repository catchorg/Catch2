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

        IStreamingReporterPtr create( std::string const& name, IConfig const* config ) const override;

        void registerReporter( std::string const& name, IReporterFactoryPtr factory );
        void registerListener( IReporterFactoryPtr factory );

        FactoryMap const& getFactories() const override;
        Listeners const& getListeners() const override;

    private:
        FactoryMap m_factories;
        Listeners m_listeners;
    };
}

#endif // CATCH_REPORTER_REGISTRY_HPP_INCLUDED
