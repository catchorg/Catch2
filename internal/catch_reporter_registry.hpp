/*
 *  catch_registry.hpp
 *  Catch
 *
 *  Created by Phil on 29/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_REGISTRY_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_REGISTRY_HPP_INCLUDED

#include "catch_ireporterregistry.h"

#include <map>

namespace Catch
{
                
    class ReporterRegistry : public IReporterRegistry
    {
    public:
        
        ~ReporterRegistry()
        {
            FactoryMap::const_iterator it =  m_factories.begin();
            FactoryMap::const_iterator itEnd =  m_factories.end();
            for(; it != itEnd; ++it )
            {
                delete it->second;
            }        
        }
        
        virtual IReporter* create( const std::string& name, const IReporterConfig& config ) const
        {
            FactoryMap::const_iterator it =  m_factories.find( name );
            if( it == m_factories.end() )
                return NULL;
            return it->second->create( config );
        }
        
        void registerReporter( const std::string& name, IReporterFactory* factory )
        {
            m_factories.insert( std::make_pair( name, factory ) );
        }        

        const FactoryMap& getFactories() const
        {
            return m_factories;
        }

    private:
        FactoryMap m_factories;
    };
    
    template<typename T>
    class ReporterFactory : public IReporterFactory
    {
        virtual IReporter* create( const IReporterConfig& config ) const
        {
            return new T( config );
        }
        virtual std::string getDescription() const
        {
            return T::getDescription();
        }
    };
    
    template<typename T>
    struct ReporterRegistrar
    {
        ReporterRegistrar( const std::string& name )
        {
            Hub::getReporterRegistry().registerReporter( name, new ReporterFactory<T>() );
        }
    }; 
}

#define CATCH_REGISTER_REPORTER( name, reporterType ) Catch::ReporterRegistrar<reporterType> catch_internal_RegistrarFor##reporterType( name );

#endif // TWOBLUECUBES_CATCH_REPORTER_REGISTRY_HPP_INCLUDED