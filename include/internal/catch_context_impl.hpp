/*
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CONTEXT_IMPL_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CONTEXT_IMPL_HPP_INCLUDED

#include "catch_runner_impl.hpp"

#include "catch_context.h"
#include "catch_stream.hpp"

namespace Catch {

    template <typename Runner, typename ResultCapture>
    class Context : public IMutableContext {

        Context() : m_config( NULL ), m_runner( &nullRunner ), m_resultCapture( &nullResultCapture ) {}
        Context( Context const& );
        void operator=( Context const& );

    public: // IContext
        virtual IResultCapture& getResultCapture() {
            return *m_resultCapture;
        }
        virtual IRunner& getRunner() {
            return *m_runner;
        }
        virtual size_t getGeneratorIndex( std::string const& fileInfo, size_t totalSize ) {
            return getGeneratorsForCurrentTest()
            .getGeneratorInfo( fileInfo, totalSize )
            .getCurrentIndex();
        }
        virtual bool advanceGeneratorsForCurrentTest() {
            IGeneratorsForTest* generators = findGeneratorsForCurrentTest();
            return generators && generators->moveNext();
        }

        virtual Ptr<IConfig const> getConfig() const {
            return m_config;
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
        IGeneratorsForTest* findGeneratorsForCurrentTest() {
            std::string testName = getResultCapture().getCurrentTestName();

            std::map<std::string, IGeneratorsForTest*>::const_iterator it =
            m_generatorsByTestName.find( testName );
            return it != m_generatorsByTestName.end()
                ? it->second
                : NULL;
        }

        IGeneratorsForTest& getGeneratorsForCurrentTest() {
            IGeneratorsForTest* generators = findGeneratorsForCurrentTest();
            if( !generators ) {
                std::string testName = getResultCapture().getCurrentTestName();
                generators = createGeneratorsForTest();
                m_generatorsByTestName.insert( std::make_pair( testName, generators ) );
            }
            return *generators;
        }

    private:
        Ptr<IConfig const> m_config;
        IRunner* m_runner;
        IResultCapture* m_resultCapture;
        std::map<std::string, IGeneratorsForTest*> m_generatorsByTestName;
        
        static ResultCapture nullResultCapture;
        static Runner nullRunner;
    public:
        static Context* currentContext;
    };

    template <typename Runner, typename ResultCapture>
    ResultCapture Context<Runner, ResultCapture>::nullResultCapture;
    template <typename Runner, typename ResultCapture>
    Runner Context<Runner, ResultCapture>::nullRunner;
    template <typename Runner, typename ResultCapture>
    Context<Runner,ResultCapture>* Context<Runner, ResultCapture>::currentContext = NULL;

    typedef Context<NullRunner, NullResultCapture> DefaultContext;
    INTERNAL_CATCH_INLINE IMutableContext& getCurrentMutableContext() {
        if( !DefaultContext::currentContext )
            DefaultContext::currentContext = new DefaultContext();
        return *DefaultContext::currentContext;
    }
    INTERNAL_CATCH_INLINE IContext& getCurrentContext() {
        return getCurrentMutableContext();
    }

    INTERNAL_CATCH_INLINE Stream createStream( std::string const& streamName ) {
        if( streamName == "stdout" ) return Stream( std::cout.rdbuf(), false );
        if( streamName == "stderr" ) return Stream( std::cerr.rdbuf(), false );
        if( streamName == "debug" ) return Stream( new StreamBufImpl<OutputDebugWriter>, true );

        throw std::domain_error( "Unknown stream: " + streamName );
    }

    INTERNAL_CATCH_INLINE void cleanUpContext() {
        delete DefaultContext::currentContext;
        DefaultContext::currentContext = NULL;
    }
}

#endif // TWOBLUECUBES_CATCH_CONTEXT_IMPL_HPP_INCLUDED
