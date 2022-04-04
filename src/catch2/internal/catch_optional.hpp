
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_OPTIONAL_HPP_INCLUDED
#define CATCH_OPTIONAL_HPP_INCLUDED

#include <cassert>

namespace Catch {

    // An optional type
    template<typename T>
    class Optional {
    public:
        Optional() : nullableValue( nullptr ) {}
        Optional( T const& _value )
        : nullableValue( new( storage ) T( _value ) )
        {}
        Optional( Optional const& _other )
        : nullableValue( _other ? new( storage ) T( *_other ) : nullptr )
        {}

        ~Optional() {
            reset();
        }

        Optional& operator= ( Optional const& _other ) {
            if( &_other != this ) {
                reset();
                if( _other )
                    nullableValue = new( storage ) T( *_other );
            }
            return *this;
        }
        Optional& operator = ( T const& _value ) {
            reset();
            nullableValue = new( storage ) T( _value );
            return *this;
        }

        void reset() {
            if( nullableValue )
                nullableValue->~T();
            nullableValue = nullptr;
        }

        T& operator*() {
            assert(nullableValue);
            return *nullableValue;
        }
        T const& operator*() const {
            assert(nullableValue);
            return *nullableValue;
        }
        T* operator->() {
            assert(nullableValue);
            return nullableValue;
        }
        const T* operator->() const {
            assert(nullableValue);
            return nullableValue;
        }

        T valueOr( T const& defaultValue ) const {
            return nullableValue ? *nullableValue : defaultValue;
        }

        bool some() const { return nullableValue != nullptr; }
        bool none() const { return nullableValue == nullptr; }

        bool operator !() const { return nullableValue == nullptr; }
        explicit operator bool() const {
            return some();
        }

        friend bool operator==(Optional const& a, Optional const& b) {
            if (a.none() && b.none()) {
                return true;
            } else if (a.some() && b.some()) {
                return *a == *b;
            } else {
                return false;
            }
        }
        friend bool operator!=(Optional const& a, Optional const& b) {
            return !( a == b );
        }

    private:
        T *nullableValue;
        alignas(alignof(T)) char storage[sizeof(T)];
    };

} // end namespace Catch

#endif // CATCH_OPTIONAL_HPP_INCLUDED
