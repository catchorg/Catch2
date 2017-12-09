/*
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "catch_context.h"
#include "catch_common.h"

namespace Catch {

    class Context : public IMutableContext, NonCopyable {

    public: // IContext
        virtual IResultCapture* getResultCapture() override {
            return m_resultCapture;
        }
        virtual IRunner* getRunner() override {
            return m_runner;
        }

        virtual IConfigPtr const& getConfig() const override {
            return m_config;
        }

        virtual ~Context() override;

    public: // IMutableContext
        virtual void setResultCapture( IResultCapture* resultCapture ) override {
            m_resultCapture = resultCapture;
        }
        virtual void setRunner( IRunner* runner ) override {
            m_runner = runner;
        }
        virtual void setConfig( IConfigPtr const& config ) override {
            m_config = config;
        }

        friend IMutableContext& getCurrentMutableContext();

    private:
        IConfigPtr m_config;
        IRunner* m_runner = nullptr;
        IResultCapture* m_resultCapture = nullptr;
    };

    IMutableContext *IMutableContext::currentContext = nullptr;

    void IMutableContext::createContext()
    {
        currentContext = new Context();
    }

    void cleanUpContext() {
        delete IMutableContext::currentContext;
        IMutableContext::currentContext = nullptr;
    }
    IContext::~IContext() = default;
    IMutableContext::~IMutableContext() = default;
    Context::~Context() = default;
}
