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

#include "catch_testcaseinfo.hpp"
#include "catch_resultinfo.hpp"
#include "catch_common.h"

#include <map>
#include <iostream>
#include <fstream>

namespace Catch
{
    class ReporterConfig
    {
    private:
        ReporterConfig( const ReporterConfig& other );
        ReporterConfig& operator = ( const ReporterConfig& other );
        
    public:
        
        struct Include { enum What
            {
                FailedOnly, 
                SuccessfulResults
            }; };
        
    public:
        
        ///////////////////////////////////////////////////////////////////////////
        explicit ReporterConfig( Include::What includeWhat = Include::FailedOnly )
        :   m_includeWhat( includeWhat ),
            m_os( std::cout.rdbuf() )
        {
        }
        
        ///////////////////////////////////////////////////////////////////////////
        bool includeSuccessfulResults() const
        {
            return m_includeWhat == Include::SuccessfulResults;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        void setIncludeWhat(Include::What includeWhat )
        {
            m_includeWhat = includeWhat;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        std::ostream& stream() const
        {
            return m_os;
        }        
        
        ///////////////////////////////////////////////////////////////////////////
        void setStreamBuf( std::streambuf* buf )
        {
            m_os.rdbuf( buf );
        }        
        
    private:
        Include::What m_includeWhat;
        
        mutable std::ostream m_os;
    };
    
    struct ITestReporter
    {
        virtual ~ITestReporter(){}
        
        // !TBD
        // StartTesting
        // EndTesting
        // StartGroup
        // EndGroup
        // StartSection
        // EndSection
        virtual void StartTestCase( const TestCaseInfo& testInfo ) = 0;
        virtual void Result( const ResultInfo& result ) = 0;
        virtual void EndTestCase( const TestCaseInfo& testInfo ) = 0;
    };
        
    struct IReporterFactory
    {
        virtual ~IReporterFactory(){}
        
        virtual ITestReporter* create( const ReporterConfig& config ) = 0;
        virtual std::string getDescription() const = 0;
    };
    
    template<typename T>
    class ReporterFactory : public IReporterFactory
    {
        virtual ITestReporter* create( const ReporterConfig& config )
        {
            return new T( config );
        }
        virtual std::string getDescription() const
        {
            return T::getDescription();
        }
    };
    
    class ReporterRegistry
    {
    public:
        
        static ReporterRegistry& instance()
        {
            static ReporterRegistry instance;
            return instance;
        }
        
        ~ReporterRegistry()
        {
            FactoryMap::const_iterator it =  m_factories.begin();
            FactoryMap::const_iterator itEnd =  m_factories.end();
            for(; it != itEnd; ++it )
            {
                delete it->second;
            }        
        }
        
        ITestReporter* create( const std::string& name, const ReporterConfig& config )
        {
            FactoryMap::const_iterator it =  m_factories.find( name );
            if( it == m_factories.end() )
                return NULL;
            return it->second->create( config );
        }
        
        template<typename T>
        void registerReporter( const std::string& name )
        {
            m_factories.insert( std::make_pair( name, new ReporterFactory<T>() ) );
        }
        
        typedef std::map<std::string, IReporterFactory*> FactoryMap;

        const FactoryMap& getFactories() const
        {
            return m_factories;
        }

    private:
        FactoryMap m_factories;
    };
    
    template<typename T>
    struct ReporterRegistrar
    {
        ReporterRegistrar( const std::string& name )
        {
            ReporterRegistry::instance().registerReporter<T>( name );
        }
    };    
}

#define CATCH_REGISTER_REPORTER( name, reporterType ) Catch::ReporterRegistrar<reporterType> INTERNAL_CATCH_UNIQUE_NAME(  catch_internal_ReporterReg )( name );

#endif // TWOBLUECUBES_CATCH_REPORTER_REGISTRY_HPP_INCLUDED