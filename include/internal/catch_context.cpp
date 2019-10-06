/*
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "catch_context.h"
#include "catch_common.h"
#include "catch_random_number_generator.h"

namespace Catch {

    class Context : public IMutableContext, NonCopyable {

    public: // IContext
        IResultCapture* getResultCapture() override {
            return m_resultCapture;
        }
        IRunner* getRunner() override {
            return m_runner;
        }

        IConfigPtr const& getConfig() const override {
            return m_config;
        }

        ~Context() override;

    public: // IMutableContext
        void setResultCapture( IResultCapture* resultCapture ) override {
            m_resultCapture = resultCapture;
        }
        void setRunner( IRunner* runner ) override {
            m_runner = runner;
        }
        void setConfig( IConfigPtr const& config ) override {
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


    SimplePcg32& rng() {
        static SimplePcg32 s_rng;
        return s_rng;
    }

}
