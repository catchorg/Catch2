/*
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "catch_runner_impl.hpp"

#include "catch_context.h"
#include "catch_stream.hpp"

namespace Catch {
    
    namespace {
        Context* currentContext = NULL;
    }
    IMutableContext& getCurrentMutableContext() {
        if( !currentContext )
            currentContext = new Context();
        return *currentContext;
    }
    IContext& getCurrentContext() {
        return getCurrentMutableContext();
    }
 
    Context::Context()
    :   m_config( NULL )
    {}

    void Context::cleanUp() {
        delete currentContext;
        currentContext = NULL;
    }

    void Context::setRunner( IRunner* runner ) {
        m_runner = runner;
    }

    void Context::setResultCapture( IResultCapture* resultCapture ) {
        m_resultCapture = resultCapture;
    }

    const IConfig* Context::getConfig() const {
        return m_config;
    }
    void Context::setConfig( const IConfig* config ) {
        m_config = config;
    }
    
    IResultCapture& Context::getResultCapture() {
        return *m_resultCapture;
    }

    IRunner& Context::getRunner() {
        return *m_runner;
    }
    
    std::streambuf* Context::createStreamBuf( const std::string& streamName ) {
        if( streamName == "stdout" ) return std::cout.rdbuf();
        if( streamName == "stderr" ) return std::cerr.rdbuf();
        if( streamName == "debug" ) return new StreamBufImpl<OutputDebugWriter>;
        
        throw std::domain_error( "Unknown stream: " + streamName );
    }

    IGeneratorsForTest* Context::findGeneratorsForCurrentTest() {
        std::string testName = getResultCapture().getCurrentTestName();
        
        std::map<std::string, IGeneratorsForTest*>::const_iterator it =
            m_generatorsByTestName.find( testName );
        return it != m_generatorsByTestName.end()
            ? it->second
            : NULL;
    }
    
    IGeneratorsForTest& Context::getGeneratorsForCurrentTest() {
        IGeneratorsForTest* generators = findGeneratorsForCurrentTest();
        if( !generators ) {
            std::string testName = getResultCapture().getCurrentTestName();
            generators = createGeneratorsForTest();
            m_generatorsByTestName.insert( std::make_pair( testName, generators ) );
        }
        return *generators;
    }
    
    size_t Context::getGeneratorIndex( const std::string& fileInfo, size_t totalSize ) {
        return getGeneratorsForCurrentTest()
            .getGeneratorInfo( fileInfo, totalSize )
            .getCurrentIndex();
    }

    bool Context::advanceGeneratorsForCurrentTest() {
        IGeneratorsForTest* generators = findGeneratorsForCurrentTest();
        return generators && generators->moveNext();
    }
}
