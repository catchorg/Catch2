/*
 *  catch_hub.h
 *  Test
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
    struct TestCaseInfo;
    struct IResultListener;
    struct ITestCaseRegistry
    {
    public:
        
        virtual void registerTest
            ( const TestCaseInfo& testInfo 
            ) = 0;

        virtual const std::vector<TestCaseInfo>& getAllTests
            () const = 0;
    };
    
    class Hub
    {
        Hub();
        static Hub& me();
        
    public:
        
        static IResultListener& getListener();
        static IReporterRegistry& getReporterRegistry();
        static ITestCaseRegistry& getTestCaseRegistry();
        
    private:
        std::auto_ptr<IReporterRegistry> m_reporterRegistry;
        std::auto_ptr<ITestCaseRegistry> m_testCaseRegistry;
    };
}

#endif // TWOBLUECUBES_CATCH_HUB_H_INCLUDED