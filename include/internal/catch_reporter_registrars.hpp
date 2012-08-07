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

namespace Catch {    

    template<typename T>
    class ReporterRegistrar {
    
        class ReporterFactory : public IReporterFactory {

            virtual IReporter* create( const ReporterConfig& config ) const {
                return new T( config );
            }
            
            virtual std::string getDescription() const {
                return T::getDescription();
            }
        };
        
    public:

        ReporterRegistrar( const std::string& name ) {
            getMutableRegistryHub().registerReporter( name, new ReporterFactory() );
        }
    }; 
}

#define INTERNAL_CATCH_REGISTER_REPORTER( name, reporterType ) \
    Catch::ReporterRegistrar<reporterType> catch_internal_RegistrarFor##reporterType( name );

#endif // TWOBLUECUBES_CATCH_REPORTER_REGISTRARS_HPP_INCLUDED
