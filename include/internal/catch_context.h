/*
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CONTEXT_H_INCLUDED
#define TWOBLUECUBES_CATCH_CONTEXT_H_INCLUDED

#include "catch_interfaces_generators.h"

#include <memory>
#include <vector>
#include <stdlib.h>

namespace Catch {

    class TestCaseInfo;
    struct IResultCapture;
    struct IRunner;
    struct IGeneratorsForTest;
    struct IConfig;

    class StreamBufBase : public std::streambuf {
    public:
        virtual ~StreamBufBase();
    };
    
    struct IContext
    {
        virtual ~IContext();
        
        virtual IResultCapture& getResultCapture() = 0;
        virtual IRunner& getRunner() = 0;
        virtual size_t getGeneratorIndex( const std::string& fileInfo, size_t totalSize ) = 0;
        virtual bool advanceGeneratorsForCurrentTest() = 0;
        virtual const IConfig* getConfig() const = 0;
    };
    
    struct IMutableContext : IContext
    {
        virtual ~IMutableContext();
        virtual void setResultCapture( IResultCapture* resultCapture ) = 0;
        virtual void setRunner( IRunner* runner ) = 0;
        virtual void setConfig( const IConfig* config ) = 0;
    };

    IContext& getCurrentContext();
    IMutableContext& getCurrentMutableContext();
    void cleanUpContext();
    std::streambuf* createStreamBuf( const std::string& streamName );

}

#endif // TWOBLUECUBES_CATCH_CONTEXT_H_INCLUDED
