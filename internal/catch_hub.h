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

#include <memory>
#include <vector>

#include "catch_interfaces_reporter.h"

namespace Catch
{
    class TestCaseInfo;
    struct IResultCapture;
    struct ITestCaseRegistry;
    struct IRunner;

    class StreamBufBase : public std::streambuf
    {
    };
    
    class Hub
    {
        Hub();
        
        static Hub& me();
        
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
        
        static std::streambuf* createStreamBuf
            ( const std::string& streamName
            );
        
        static IRunner& getRunner
            ();
        
    private:
        std::auto_ptr<IReporterRegistry> m_reporterRegistry;
        std::auto_ptr<ITestCaseRegistry> m_testCaseRegistry;
        IRunner* m_runner;
        IResultCapture* m_resultCapture;
    };
}

#endif // TWOBLUECUBES_CATCH_HUB_H_INCLUDED