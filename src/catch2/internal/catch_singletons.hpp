
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_SINGLETONS_HPP_INCLUDED
#define CATCH_SINGLETONS_HPP_INCLUDED

#include <catch2/internal/catch_dll_public.hpp>

namespace Catch {

    struct CATCH_DLL_PUBLIC ISingleton {
        virtual ~ISingleton(); // = default
    };

    void addSingleton( ISingleton* singleton );
    void cleanupSingletons();

    template <typename SingletonImplT,
              typename InterfaceT = SingletonImplT,
              typename MutableInterfaceT = InterfaceT>
    class CATCH_DLL_PUBLIC Singleton : SingletonImplT, public ISingleton {

        static auto getInternal() -> Singleton* {
            static Singleton* s_instance = nullptr;
            if( !s_instance ) {
                s_instance = new Singleton;
                addSingleton( s_instance );
            }
            return s_instance;
        }

    public:
        static auto get() -> InterfaceT const& {
            return *getInternal();
        }
        static auto getMutable() -> MutableInterfaceT& {
            return *getInternal();
        }
    };

} // namespace Catch

#endif // CATCH_SINGLETONS_HPP_INCLUDED
