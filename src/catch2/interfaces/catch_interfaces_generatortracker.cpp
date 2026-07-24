
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/interfaces/catch_interfaces_generatortracker.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <string>

namespace Catch {
    namespace Generators {

        bool GeneratorUntypedBase::countedNext() {
            auto ret = next();
            if ( ret ) {
                m_stringReprCache.clear();
                ++m_currentElementIndex;
            }
            return ret;
        }

        void GeneratorUntypedBase::skipToNthElementImpl( std::size_t n ) {
            for ( size_t i = m_currentElementIndex; i < n; ++i ) {
                bool isValid = next();
                if ( !isValid ) {
                    Detail::throw_generator_exception(
                        "Could not jump to Nth element: not enough elements" );
                }
            }
        }

        void GeneratorUntypedBase::skipToNthElement( std::size_t n ) {
            if ( n < m_currentElementIndex ) {
                Detail::throw_generator_exception(
                    "Tried to jump generator backwards" );
            }
            if ( n == m_currentElementIndex ) { return; }

            skipToNthElementImpl(n);
            // Fixup tracking after moving the generator forward
            //  * Ensure that the correct element index is set after skipping
            //  * Invalidate cache
            m_currentElementIndex = n;
            m_stringReprCache.clear();
        }

        StringRef GeneratorUntypedBase::currentElementAsString() const {
            if ( m_stringReprCache.empty() ) {
                m_stringReprCache = stringifyImpl();
            }
            return m_stringReprCache;
        }

        bool GeneratorUntypedBase::isFinite() const { return true; }

    } // namespace Generators
} // namespace Catch
