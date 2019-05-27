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

template <typename> struct true_given : std::true_type {};
struct is_callable_tester {
    template <typename Fun, typename... Args>
    true_given<decltype(std::declval<Fun>()(std::declval<Args>()...))> static test(int);
    template <typename...>
    std::false_type static test(...);
};

template <typename T>
struct is_callable;

template <typename Fun, typename... Args>
struct is_callable<Fun(Args...)> : decltype(is_callable_tester::test<Fun, Args...>(0)) {};

} // namespace Catch

namespace mpl_{
    struct na;
}

#endif // TWOBLUECUBES_CATCH_META_HPP_INCLUDED
