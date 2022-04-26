
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_CONTEXT_HPP_INCLUDED
#define CATCH_CONTEXT_HPP_INCLUDED

#include <catch2/internal/catch_dll_public.hpp>

namespace Catch {

    struct IResultCapture;
    struct IConfig;

    struct CATCH_DLL_PUBLIC IContext {
        virtual ~IContext(); // = default

        virtual IResultCapture* getResultCapture() = 0;
        virtual IConfig const* getConfig() const = 0;
    };

    struct CATCH_DLL_PUBLIC IMutableContext : IContext {
        virtual ~IMutableContext(); // = default
        virtual void setResultCapture( IResultCapture* resultCapture ) = 0;
        virtual void setConfig( IConfig const* config ) = 0;

    private:
        static IMutableContext *currentContext;
        friend IMutableContext& getCurrentMutableContext();
        friend void cleanUpContext();
        static void createContext();
    };

    inline IMutableContext& getCurrentMutableContext()
    {
        if( !IMutableContext::currentContext )
            IMutableContext::createContext();
        // NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn)
        return *IMutableContext::currentContext;
    }

    inline IContext& getCurrentContext()
    {
        return getCurrentMutableContext();
    }

    void cleanUpContext();

    class CATCH_DLL_PUBLIC SimplePcg32;
    CATCH_DLL_PUBLIC SimplePcg32& rng();
}

#endif // CATCH_CONTEXT_HPP_INCLUDED
