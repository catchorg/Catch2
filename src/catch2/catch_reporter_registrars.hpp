
/*
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_REGISTRARS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_REGISTRARS_HPP_INCLUDED

#include <catch2/interfaces/catch_interfaces_registry_hub.hpp>

namespace Catch {

    template <typename T>
    class ReporterFactory : public IReporterFactory {

        IStreamingReporterPtr create( ReporterConfig const& config ) const override {
            return std::make_unique<T>( config );
        }

        std::string getDescription() const override {
            return T::getDescription();
        }
    };


    template<typename T>
    class ReporterRegistrar {
    public:
        explicit ReporterRegistrar( std::string const& name ) {
            getMutableRegistryHub().registerReporter( name, std::make_unique<ReporterFactory<T>>() );
        }
    };

    template<typename T>
    class ListenerRegistrar {

        class ListenerFactory : public IReporterFactory {

            IStreamingReporterPtr create( ReporterConfig const& config ) const override {
                return std::make_unique<T>(config);
            }
            std::string getDescription() const override {
                return std::string();
            }
        };

    public:

        ListenerRegistrar() {
            getMutableRegistryHub().registerListener( std::make_unique<ListenerFactory>() );
        }
    };
}

#if !defined(CATCH_CONFIG_DISABLE)

#define CATCH_REGISTER_REPORTER( name, reporterType ) \
    CATCH_INTERNAL_START_WARNINGS_SUPPRESSION         \
    CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS          \
    namespace{ Catch::ReporterRegistrar<reporterType> catch_internal_RegistrarFor##reporterType( name ); } \
    CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION

#define CATCH_REGISTER_LISTENER( listenerType ) \
    CATCH_INTERNAL_START_WARNINGS_SUPPRESSION   \
    CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS    \
    namespace{ Catch::ListenerRegistrar<listenerType> catch_internal_RegistrarFor##listenerType; } \
    CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION
#else // CATCH_CONFIG_DISABLE

#define CATCH_REGISTER_REPORTER(name, reporterType)
#define CATCH_REGISTER_LISTENER(listenerType)

#endif // CATCH_CONFIG_DISABLE

#endif // TWOBLUECUBES_CATCH_REPORTER_REGISTRARS_HPP_INCLUDED
