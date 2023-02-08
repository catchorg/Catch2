
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// Adapted from donated nonius code.

#ifndef CATCH_CONSTRUCTOR_HPP_INCLUDED
#define CATCH_CONSTRUCTOR_HPP_INCLUDED

#include <catch2/internal/catch_move_and_forward.hpp>

#include <type_traits>

namespace Catch {
    namespace Benchmark {
        namespace Detail {
            template <typename T, bool Destruct>
            struct ObjectStorage
            {
                ObjectStorage() = default;

                ObjectStorage(const ObjectStorage& other)
                {
                    new(&storage) T(other.stored_object());
                }

                ObjectStorage(ObjectStorage&& other)
                {
                    new(&storage) T(CATCH_MOVE(other.stored_object()));
                }

                ~ObjectStorage() { destruct_on_exit<T>(); }

                template <typename... Args>
                void construct(Args&&... args)
                {
                    new (&storage) T(CATCH_FORWARD(args)...);
                }

                template <bool AllowManualDestruction = !Destruct>
                std::enable_if_t<AllowManualDestruction> destruct()
                {
                    stored_object().~T();
                }

            private:
                // If this is a constructor benchmark, destruct the underlying object
                template <typename U>
                void destruct_on_exit(std::enable_if_t<Destruct, U>* = nullptr) { destruct<true>(); }
                // Otherwise, don't
                template <typename U>
                void destruct_on_exit(std::enable_if_t<!Destruct, U>* = nullptr) { }

                T& stored_object() {
                    return *CATCH_LAUNDER(reinterpret_cast<T*>( &storage ));
                }

                T const& stored_object() const {
                    return *CATCH_LAUNDER(reinterpret_cast<T const*>(&storage));
                }


                struct { alignas( T ) unsigned char data[sizeof( T )]; }  storage{};
            };
        } // namespace Detail

        template <typename T>
        using storage_for = Detail::ObjectStorage<T, true>;

        template <typename T>
        using destructable_object = Detail::ObjectStorage<T, false>;
    } // namespace Benchmark
} // namespace Catch

#endif // CATCH_CONSTRUCTOR_HPP_INCLUDED
