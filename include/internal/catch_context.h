/*
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CONTEXT_H_INCLUDED
#define TWOBLUECUBES_CATCH_CONTEXT_H_INCLUDED

#include "catch_interfaces_reporter.h"
#include "catch_interfaces_config.h"
#include "catch_interfaces_generators.h"

#include <memory>
#include <vector>
#include <stdlib.h>

namespace Catch {

    class TestCaseInfo;
    struct IResultCapture;
    struct IRunner;
    struct IGeneratorsForTest;

    class StreamBufBase : public std::streambuf{};
    
    struct IContext
    {
        virtual ~IContext(){}
        
        virtual IResultCapture& getResultCapture() = 0;
        virtual IRunner& getRunner() = 0;
        virtual size_t getGeneratorIndex( const std::string& fileInfo, size_t totalSize ) = 0;
        virtual bool advanceGeneratorsForCurrentTest() = 0;
        virtual const IConfig* getConfig() const = 0;
    };
    
    struct IMutableContext : IContext
    {
        virtual void setResultCapture( IResultCapture* resultCapture ) = 0;
        virtual void setRunner( IRunner* runner ) = 0;
        virtual void setConfig( const IConfig* config ) = 0;
    };

    IContext& getCurrentContext();
    IMutableContext& getCurrentMutableContext();    

    class Context : public IMutableContext {
    
        Context();
        Context( const Context& );
        void operator=( const Context& );

    public: // IContext
        virtual IResultCapture& getResultCapture();
        virtual IRunner& getRunner();
        virtual size_t getGeneratorIndex( const std::string& fileInfo, size_t totalSize );
        virtual bool advanceGeneratorsForCurrentTest();
        virtual const IConfig* getConfig() const;

    public: // IMutableContext
        virtual void setResultCapture( IResultCapture* resultCapture );
        virtual void setRunner( IRunner* runner );
        virtual void setConfig( const IConfig* config );
    
    public: // Statics
        static std::streambuf* createStreamBuf( const std::string& streamName );        
        static void cleanUp();
        
        friend IMutableContext& getCurrentMutableContext();

    private:
        IGeneratorsForTest* findGeneratorsForCurrentTest();
        IGeneratorsForTest& getGeneratorsForCurrentTest();

    private:        
        IRunner* m_runner;
        IResultCapture* m_resultCapture;
        const IConfig* m_config;
        std::map<std::string, IGeneratorsForTest*> m_generatorsByTestName;
    };
}

#endif // TWOBLUECUBES_CATCH_CONTEXT_H_INCLUDED
