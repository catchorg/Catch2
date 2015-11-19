/*
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CONTEXT_IMPL_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CONTEXT_IMPL_HPP_INCLUDED

#include "catch_run_context.hpp"

#include "catch_context.h"
#include "catch_stream.hpp"

namespace Catch {

    class Context : public IMutableContext {

        Context() : m_config( CATCH_NULL ), m_runner( CATCH_NULL ), m_resultCapture( CATCH_NULL ) {}
        Context( Context const& );
        void operator=( Context const& );

    public: // IContext
        virtual IResultCapture* getResultCapture() {
            return m_resultCapture;
        }
        virtual IRunner* getRunner() {
            return m_runner;
        }
        virtual IConfig const* getConfig() const {
            return m_config.get();
        }

    public: // IMutableContext
        virtual void setResultCapture( IResultCapture* resultCapture ) {
            m_resultCapture = resultCapture;
        }
        virtual void setRunner( IRunner* runner ) {
            m_runner = runner;
        }
        virtual void setConfig( Ptr<IConfig const> const& config ) {
            m_config = config;
        }

        friend IMutableContext& getCurrentMutableContext();

    private:
        Ptr<IConfig const> m_config;
        IRunner* m_runner;
        IResultCapture* m_resultCapture;
    };

    namespace {
        Context* currentContext = CATCH_NULL;
    }
    IMutableContext& getCurrentMutableContext() {
        if( !currentContext )
            currentContext = new Context();
        return *currentContext;
    }
    IContext& getCurrentContext() {
        return getCurrentMutableContext();
    }

    void cleanUpContext() {
        delete currentContext;
        currentContext = CATCH_NULL;
    }

    IConfig const* getCurrentConfig() {
        return currentContext
            ? currentContext->getConfig()
            : CATCH_NULL;
    }
    
}

#endif // TWOBLUECUBES_CATCH_CONTEXT_IMPL_HPP_INCLUDED
