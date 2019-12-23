/*
 *  Created by Joachim on 16/04/2019.
 *  Adapted from donated nonius code.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// Constructor and destructor helpers

#ifndef TWOBLUECUBES_CATCH_CONSTRUCTOR_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CONSTRUCTOR_HPP_INCLUDED

#include <type_traits>

namespace Catch {
    namespace Benchmark {
        namespace Detail {
            template <typename T, bool Destruct>
            struct ObjectStorage
            {
                using TStorage = typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type;

                ObjectStorage() : data() {}

                ObjectStorage(const ObjectStorage& other)
                {
                    new(&data) T(other.stored_object());
                }

                ObjectStorage(ObjectStorage&& other)
                {
                    new(&data) T(std::move(other.stored_object()));
                }

                ~ObjectStorage() { destruct_on_exit<T>(); }

                template <typename... Args>
                void construct(Args&&... args)
                {
                    new (&data) T(std::forward<Args>(args)...);
                }

                template <bool AllowManualDestruction = !Destruct>
                typename std::enable_if<AllowManualDestruction>::type destruct()
                {
                    stored_object().~T();
                }

            private:
                // If this is a constructor benchmark, destruct the underlying object
                template <typename U>
                void destruct_on_exit(typename std::enable_if<Destruct, U>::type* = 0) { destruct<true>(); }
                // Otherwise, don't
                template <typename U>
                void destruct_on_exit(typename std::enable_if<!Destruct, U>::type* = 0) { }

                T& stored_object() {
                    return *static_cast<T*>(static_cast<void*>(&data));
                }

                T const& stored_object() const {
                    return *static_cast<T*>(static_cast<void*>(&data));
                }


                TStorage data;
            };
        }

        template <typename T>
        using storage_for = Detail::ObjectStorage<T, true>;

        template <typename T>
        using destructable_object = Detail::ObjectStorage<T, false>;
    }
}

#endif // TWOBLUECUBES_CATCH_CONSTRUCTOR_HPP_INCLUDED
