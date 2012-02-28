/*
 *  catch_interfaces_reporter.h
 *  Test
 *
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef TWOBLUECUBES_CATCH_IREPORTERREGISTRY_INCLUDED
#define TWOBLUECUBES_CATCH_IREPORTERREGISTRY_INCLUDED

#include "catch_common.h"
#include "catch_totals.hpp"

#include <string>
#include <ostream>
#include <map>

namespace Catch
{
    ///////////////////////////////////////////////////////////////////////////
    struct IReporterConfig
    {
        virtual ~IReporterConfig
        ()
        {}
        
        virtual std::ostream& stream
            () const = 0;
        
        virtual bool includeSuccessfulResults
            () const = 0;

        virtual std::string getName
            () const = 0;
    };
    
    class TestCaseInfo;
    class ResultInfo;
    
    ///////////////////////////////////////////////////////////////////////////
    struct IReporter : NonCopyable
    {
        virtual ~IReporter
            (){}
        
        virtual bool shouldRedirectStdout
            () const = 0;
        
        virtual void StartTesting
            () = 0;
        
        virtual void EndTesting
            (   const Totals& totals
            ) = 0;
        
        virtual void StartGroup
            (   const std::string& groupName
            ) = 0;
        
        virtual void EndGroup
            (   const std::string& groupName, 
                const Totals& totals
            ) = 0;
        
        virtual void StartSection
            (   const std::string& sectionName, 
                const std::string description 
            ) = 0;
        
        virtual void EndSection
            (   const std::string& sectionName, 
                const Counts& assertions 
            ) = 0;
        
        virtual void StartTestCase
            (   const TestCaseInfo& testInfo 
            ) = 0;
        
        virtual void EndTestCase
            (   const TestCaseInfo& testInfo, 
                const Totals& totals,
                const std::string& stdOut, 
                const std::string& stdErr 
            ) = 0;
        
        virtual void Result
            (   const ResultInfo& result 
            ) = 0;
    };
    
    ///////////////////////////////////////////////////////////////////////////
    struct IReporterFactory
    {
        virtual ~IReporterFactory
            (){}
        
        virtual IReporter* create
            (   const IReporterConfig& config 
            ) const = 0;
        
        virtual std::string getDescription
            () const = 0;
    };

    ///////////////////////////////////////////////////////////////////////////
    struct IReporterRegistry
    {
        typedef std::map<std::string, IReporterFactory*> FactoryMap;

        virtual ~IReporterRegistry
            (){}

        virtual IReporter* create
            (   const std::string& name, 
                const IReporterConfig& config 
            ) const = 0;
        
        virtual void registerReporter
            (   const std::string& name, 
                IReporterFactory* factory 
            ) = 0;
        
        virtual const FactoryMap& getFactories
            () const = 0;

    };
    
    ///////////////////////////////////////////////////////////////////////////
    inline std::string trim( const std::string& str )
    {
        std::string::size_type start = str.find_first_not_of( "\n\r\t " );
        std::string::size_type end = str.find_last_not_of( "\n\r\t " );
        
        return start != std::string::npos ? str.substr( start, 1+end-start ) : "";
    }
    
    
}

#endif // TWOBLUECUBES_CATCH_IREPORTERREGISTRY_INCLUDED
