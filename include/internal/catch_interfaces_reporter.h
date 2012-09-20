/*
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_IREPORTERREGISTRY_INCLUDED
#define TWOBLUECUBES_CATCH_IREPORTERREGISTRY_INCLUDED

#include "catch_common.h"
#include "catch_totals.hpp"
#include "catch_ptr.hpp"
#include "catch_config.hpp"

#include <string>
#include <ostream>
#include <map>

namespace Catch
{
    struct ReporterConfig
    {
        ReporterConfig( const std::string& _name,
                        std::ostream& _stream,
                        bool _includeSuccessfulResults,
                        const ConfigData& _fullConfig )
        :   name( _name ),
            stream( _stream ),
            includeSuccessfulResults( _includeSuccessfulResults ),
            fullConfig( _fullConfig )
        {}
        
        ReporterConfig( const ReporterConfig& other )
        :   name( other.name ),
            stream( other.stream ),
            includeSuccessfulResults( other.includeSuccessfulResults ),
            fullConfig( other.fullConfig )
        {}
        

        std::string name;
        std::ostream& stream;
        bool includeSuccessfulResults;
        ConfigData fullConfig;

    private:
        void operator=(const ReporterConfig&);
    };
    
    class TestCaseInfo;
    class ResultInfo;
    
    struct IReporter : IShared {
        virtual ~IReporter();
        virtual bool shouldRedirectStdout() const = 0;        
        virtual void StartTesting() = 0;        
        virtual void EndTesting( const Totals& totals ) = 0;        
        virtual void StartGroup( const std::string& groupName ) = 0;        
        virtual void EndGroup( const std::string& groupName, const Totals& totals ) = 0;        
        virtual void StartSection( const std::string& sectionName, const std::string& description ) = 0;
        virtual void NoAssertionsInSection( const std::string& sectionName ) = 0;
        virtual void NoAssertionsInTestCase( const std::string& testName ) = 0;
        virtual void EndSection( const std::string& sectionName, const Counts& assertions ) = 0;
        virtual void StartTestCase( const TestCaseInfo& testInfo ) = 0;        
        virtual void Aborted() = 0;
        virtual void EndTestCase( const TestCaseInfo& testInfo, const Totals& totals, const std::string& stdOut, const std::string& stdErr ) = 0;
        virtual void Result( const ResultInfo& result ) = 0;
    };
    
    struct IReporterFactory {
        virtual ~IReporterFactory();
        virtual IReporter* create( const ReporterConfig& config ) const = 0;
        virtual std::string getDescription() const = 0;
    };

    struct IReporterRegistry {
        typedef std::map<std::string, IReporterFactory*> FactoryMap;

        virtual ~IReporterRegistry();
        virtual IReporter* create( const std::string& name, const ReporterConfig& config ) const = 0;        
        virtual const FactoryMap& getFactories() const = 0;
    };
    
    inline std::string trim( const std::string& str ) {
        std::string::size_type start = str.find_first_not_of( "\n\r\t " );
        std::string::size_type end = str.find_last_not_of( "\n\r\t " );
        
        return start != std::string::npos ? str.substr( start, 1+end-start ) : "";
    }
}

#endif // TWOBLUECUBES_CATCH_IREPORTERREGISTRY_INCLUDED
