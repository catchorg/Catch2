/*
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "catch_test_case_registry_impl.hpp"
#include "catch_runner_impl.hpp"
#include "catch_generators_impl.hpp"
#include "catch_console_colour_impl.hpp"

#include "catch_exception_translator_registry.hpp"
#include "catch_context.h"
#include "catch_reporter_registry.hpp"
#include "catch_stream.hpp"

namespace Catch {
    
    Context::Context()
    :   m_reporterRegistry( new ReporterRegistry ),
        m_testCaseRegistry( new TestRegistry ),
        m_exceptionTranslatorRegistry( new ExceptionTranslatorRegistry )
    {}

    Context& Context::me() {
        Context*& hub = singleInstance();
        if( !hub )
            hub = new Context();
        return *hub;
    }

    void Context::cleanUp() {
        Context*& hub = singleInstance();
        delete hub;
        hub = NULL;
    }

    Context*& Context::singleInstance() {
        static Context* hub = NULL;        
        return hub;
    }

    void Context::setRunner( IRunner* runner ) {
        me().m_runner = runner;
    }

    void Context::setResultCapture( IResultCapture* resultCapture ) {
        me().m_resultCapture = resultCapture;
    }
    
    IResultCapture& Context::getResultCapture() {
        return *me().m_resultCapture;
    }

    IRunner& Context::getRunner() {
        return *me().m_runner;
    }
    
    IReporterRegistry& Context::getReporterRegistry() {
        return *me().m_reporterRegistry.get();
    }

    ITestCaseRegistry& Context::getTestCaseRegistry() {
        return *me().m_testCaseRegistry.get();
    }
    
    IExceptionTranslatorRegistry& Context::getExceptionTranslatorRegistry() {
        return *me().m_exceptionTranslatorRegistry.get();
    }
    
    std::streambuf* Context::createStreamBuf( const std::string& streamName ) {
        if( streamName == "stdout" ) return std::cout.rdbuf();
        if( streamName == "stderr" ) return std::cerr.rdbuf();
        if( streamName == "debug" ) return new StreamBufImpl<OutputDebugWriter>;
        
        throw std::domain_error( "Unknown stream: " + streamName );
    }

    GeneratorsForTest* Context::findGeneratorsForCurrentTest() {
        std::string testName = getResultCapture().getCurrentTestName();
        
        std::map<std::string, GeneratorsForTest*>::const_iterator it = 
            m_generatorsByTestName.find( testName );
        return it != m_generatorsByTestName.end()
            ? it->second
            : NULL;
    }
    
    GeneratorsForTest& Context::getGeneratorsForCurrentTest() {
        GeneratorsForTest* generators = findGeneratorsForCurrentTest();
        if( !generators ) {
            std::string testName = getResultCapture().getCurrentTestName();
            generators = new GeneratorsForTest();
            m_generatorsByTestName.insert( std::make_pair( testName, generators ) );
        }
        return *generators;
    }
    
    size_t Context::getGeneratorIndex( const std::string& fileInfo, size_t totalSize ) {
        return me().getGeneratorsForCurrentTest()
            .getGeneratorInfo( fileInfo, totalSize )
            .getCurrentIndex();
    }

    bool Context::advanceGeneratorsForCurrentTest() {
        GeneratorsForTest* generators = me().findGeneratorsForCurrentTest();
        return generators && generators->moveNext();
    }
}
