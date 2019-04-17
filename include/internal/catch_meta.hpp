/*
 *  Created by Jozef on 02/12/2018.
 *  Copyright 2018 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef TWOBLUECUBES_CATCH_META_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_META_HPP_INCLUDED

#include <type_traits>

namespace Catch {
    template<typename T>
    struct always_false : std::false_type {};
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_META_HPP_INCLUDED
