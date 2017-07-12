/*
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CONTEXT_IMPL_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CONTEXT_IMPL_HPP_INCLUDED

#include "catch_context.h"
#include "catch_common.h"

namespace Catch {

    class Context : public IMutableContext, NonCopyable {

    public: // IContext
        virtual IResultCapture* getResultCapture() {
            return m_resultCapture;
        }
        virtual IRunner* getRunner() {
            return m_runner;
        }

        virtual IConfigPtr getConfig() const {
            return m_config;
        }

    public: // IMutableContext
        virtual void setResultCapture( IResultCapture* resultCapture ) {
            m_resultCapture = resultCapture;
        }
        virtual void setRunner( IRunner* runner ) {
            m_runner = runner;
        }
        virtual void setConfig( IConfigPtr const& config ) {
            m_config = config;
        }

        friend IMutableContext& getCurrentMutableContext();

    private:

    private:
        IConfigPtr m_config;
        IRunner* m_runner = nullptr;
        IResultCapture* m_resultCapture = nullptr;
    };

    namespace {
        Context* currentContext = nullptr;
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
        currentContext = nullptr;
    }
}

#endif // TWOBLUECUBES_CATCH_CONTEXT_IMPL_HPP_INCLUDED
