/*
 *  catch_hub.h
 *  Catch
 *
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_HUB_H_INCLUDED
#define TWOBLUECUBES_CATCH_HUB_H_INCLUDED

#include "catch_interfaces_reporter.h"

#include <memory>
#include <vector>
#include <stdlib.h>

namespace Catch
{
    class TestCaseInfo;
    struct IResultCapture;
    struct ITestCaseRegistry;
    struct IRunner;
    struct IExceptionTranslatorRegistry;
    class GeneratorsForTest;

    class StreamBufBase : public std::streambuf
    {
    };
    
    class Hub
    {
        Hub();
        
        static Hub& me();
        
        Hub( const Hub& );
        void operator=( const Hub& );
        
    public:
        
        static void setRunner
            ( IRunner* runner 
            );
        
        static void setResultCapture
            ( IResultCapture* resultCapture 
            );
        
        static IResultCapture& getResultCapture
            ();
        
        static IReporterRegistry& getReporterRegistry
            ();
        
        static ITestCaseRegistry& getTestCaseRegistry
            ();
        
        static IExceptionTranslatorRegistry& getExceptionTranslatorRegistry
            ();
        
        static std::streambuf* createStreamBuf
            ( const std::string& streamName
            );
        
        static IRunner& getRunner
            ();
        
        static size_t getGeneratorIndex
            (   const std::string& fileInfo, 
                size_t totalSize 
            );
        
        static bool advanceGeneratorsForCurrentTest
            ();
        
        static void cleanUp
            ();
        
    private:

        static Hub*& singleInstance();
        
        GeneratorsForTest* findGeneratorsForCurrentTest
            ();
        
        GeneratorsForTest& getGeneratorsForCurrentTest
            ();
        
        std::auto_ptr<IReporterRegistry> m_reporterRegistry;
        std::auto_ptr<ITestCaseRegistry> m_testCaseRegistry;
        std::auto_ptr<IExceptionTranslatorRegistry> m_exceptionTranslatorRegistry;
        IRunner* m_runner;
        IResultCapture* m_resultCapture;
        std::map<std::string, GeneratorsForTest*> m_generatorsByTestName;
    };
}

#endif // TWOBLUECUBES_CATCH_HUB_H_INCLUDED
