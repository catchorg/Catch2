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

#include <memory>
#include <vector>
#include <stdlib.h>

namespace Catch {

    class TestCaseInfo;
    struct IResultCapture;
    struct ITestCaseRegistry;
    struct IRunner;
    struct IExceptionTranslatorRegistry;
    class GeneratorsForTest;

    class StreamBufBase : public std::streambuf{};
    
    struct IContext
    {
        virtual IResultCapture& getResultCapture() = 0;
        virtual IRunner& getRunner() = 0;
        virtual IReporterRegistry& getReporterRegistry() = 0;
        virtual ITestCaseRegistry& getTestCaseRegistry() = 0;
        virtual IExceptionTranslatorRegistry& getExceptionTranslatorRegistry() = 0;
        virtual size_t getGeneratorIndex( const std::string& fileInfo, size_t totalSize ) = 0;
        virtual bool advanceGeneratorsForCurrentTest() = 0;
    };
    
    struct IMutableContext : IContext
    {
        virtual void setResultCapture( IResultCapture* resultCapture ) = 0;
        virtual void setRunner( IRunner* runner ) = 0;
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
        virtual IReporterRegistry& getReporterRegistry();
        virtual ITestCaseRegistry& getTestCaseRegistry();
        virtual IExceptionTranslatorRegistry& getExceptionTranslatorRegistry();
        virtual size_t getGeneratorIndex( const std::string& fileInfo, size_t totalSize );
        virtual bool advanceGeneratorsForCurrentTest();        

    public: // IMutableContext
        virtual void setResultCapture( IResultCapture* resultCapture );
        virtual void setRunner( IRunner* runner );
    
    public: // Statics
        static std::streambuf* createStreamBuf( const std::string& streamName );        
        static void cleanUp();
        
        friend IMutableContext& getCurrentMutableContext();

    private:
        GeneratorsForTest* findGeneratorsForCurrentTest();        
        GeneratorsForTest& getGeneratorsForCurrentTest();

    private:        
        std::auto_ptr<IReporterRegistry> m_reporterRegistry;
        std::auto_ptr<ITestCaseRegistry> m_testCaseRegistry;
        std::auto_ptr<IExceptionTranslatorRegistry> m_exceptionTranslatorRegistry;
        IRunner* m_runner;
        IResultCapture* m_resultCapture;
        std::map<std::string, GeneratorsForTest*> m_generatorsByTestName;
    };
}

#endif // TWOBLUECUBES_CATCH_CONTEXT_H_INCLUDED
