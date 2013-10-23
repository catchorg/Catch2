/*
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_REGISTRARS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_REGISTRARS_HPP_INCLUDED

#include "catch_interfaces_registry_hub.h"
#include "catch_legacy_reporter_adapter.h"

namespace Catch {

    template<typename T>
    class LegacyReporterRegistrar {

        class ReporterFactory : public IReporterFactory {
            virtual IStreamingReporter* create( ReporterConfig const& config ) const {
                return new LegacyReporterAdapter( new T( config ) );
            }

            virtual std::string getDescription() const {
                return T::getDescription();
            }
        };

    public:

        LegacyReporterRegistrar( std::string const& name ) {
            getMutableRegistryHub().registerReporter( name, new ReporterFactory() );
        }
    };

    template<typename T>
    class ReporterRegistrar {

        class ReporterFactory : public IReporterFactory {

            // *** Please Note ***:
            // - If you end up here looking at a compiler error because it's trying to register
            // your custom reporter class be aware that the native reporter interface has changed
            // to IStreamingReporter. The "legacy" interface, IReporter, is still supported via
            // an adapter. Just use REGISTER_LEGACY_REPORTER to take advantage of the adapter.
            // However please consider updating to the new interface as the old one is now
            // deprecated and will probably be removed quite soon!
            // Please contact me via github if you have any questions at all about this.
            // In fact, ideally, please contact me anyway to let me know you've hit this - as I have
            // no idea who is actually using custom reporters at all (possibly no-one!).
            // The new interface is designed to minimise exposure to interface changes in the future.
            virtual IStreamingReporter* create( ReporterConfig const& config ) const {
                return new T( config );
            }

            virtual std::string getDescription() const {
                return T::getDescription();
            }
        };

    public:

        ReporterRegistrar( std::string const& name ) {
            getMutableRegistryHub().registerReporter( name, new ReporterFactory() );
        }
    };
}

#define INTERNAL_CATCH_REGISTER_LEGACY_REPORTER( name, reporterType ) \
    namespace{ Catch::LegacyReporterRegistrar<reporterType> catch_internal_RegistrarFor##reporterType( name ); }
#define INTERNAL_CATCH_REGISTER_REPORTER( name, reporterType ) \
    namespace{ Catch::ReporterRegistrar<reporterType> catch_internal_RegistrarFor##reporterType( name ); }

#endif // TWOBLUECUBES_CATCH_REPORTER_REGISTRARS_HPP_INCLUDED
