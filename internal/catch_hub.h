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
#include "catch_interfaces_reporter.h"

namespace Catch
{
    struct IResultListener;
    struct ITestCaseRegistry;

    class Hub
    {
        Hub();
        static Hub& me()
        {
            static Hub hub;
            return hub;
        }
    public:
        
        static IResultListener& getListener();
        static IReporterRegistry& getReporterRegistry();
        static ITestCaseRegistry& getTestCaseRegistry();
        
    private:
        std::auto_ptr<IReporterRegistry> m_reporterRegistry;
    };
}

#endif // TWOBLUECUBES_CATCH_HUB_H_INCLUDED