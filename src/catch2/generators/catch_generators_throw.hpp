
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_GENERATORS_THROW_HPP_INCLUDED
#define CATCH_GENERATORS_THROW_HPP_INCLUDED

namespace Catch {
    namespace Generators {
        namespace Detail {

            //! Throws GeneratorException with the provided message
            [[noreturn]]
            void throw_generator_exception( char const* msg );

        } // namespace Detail
    } // namespace Generators
} // namespace Catch

#endif // CATCH_GENERATORS_THROW_HPP_INCLUDED
