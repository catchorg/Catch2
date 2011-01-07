/*
 *  catch_hub_impl.hpp
 *  Test
 *
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#include "catch_hub.h"
#include "catch_reporter_registry.hpp"
#include "catch_test_case_registry_impl.hpp"

namespace Catch
{
    ///////////////////////////////////////////////////////////////////////////
    Hub::Hub
    ()
    :   m_reporterRegistry( new ReporterRegistry ),
        m_testCaseRegistry( new TestRegistry )
    {
    }
    
    ///////////////////////////////////////////////////////////////////////////
    Hub& Hub::me
    ()
    {
        static Hub hub;
        return hub;
    }

    ///////////////////////////////////////////////////////////////////////////
    IReporterRegistry& Hub::getReporterRegistry
    ()
    {
        return *me().m_reporterRegistry.get();
    }

    ///////////////////////////////////////////////////////////////////////////
    ITestCaseRegistry& Hub::getTestCaseRegistry
    ()
    {
        return *me().m_testCaseRegistry.get();
    }
}
