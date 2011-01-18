/*
 *  catch_hub_impl.hpp
 *  Catch
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
#include "catch_runner_impl.hpp"
#include "catch_stream.hpp"

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
    void Hub::setRunner( IRunner* runner )
    {
        me().m_runner = runner;
    }
    ///////////////////////////////////////////////////////////////////////////
    void Hub::setResultCapture( IResultCapture* resultCapture )
    {
        me().m_resultCapture = resultCapture;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    IResultCapture& Hub::getResultCapture
    ()
    {
        return *me().m_resultCapture;
    }

    ///////////////////////////////////////////////////////////////////////////
    IRunner& Hub::getRunner
    ()
    {
        return *me().m_runner;
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
    
    ///////////////////////////////////////////////////////////////////////////
    std::streambuf* Hub::createStreamBuf
    (
        const std::string& streamName
    )
    {
        if( streamName == "stdout" ) return std::cout.rdbuf();
        if( streamName == "stderr" ) return std::cerr.rdbuf();
        if( streamName == "debug" ) return new StreamBufImpl<OutputDebugWriter>;
        
        throw std::domain_error( "Unknown stream: " + streamName );
    }
    
}
