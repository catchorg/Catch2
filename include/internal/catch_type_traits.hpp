/*
 *  Created by Jozef on 12/11/2018.
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef TWOBLUECUBES_CATCH_TYPE_TRAITS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TYPE_TRAITS_HPP_INCLUDED

#include <type_traits>

namespace Catch{

#ifdef CATCH_CPP17_OR_GREATER
	template <typename...>
	inline constexpr auto is_unique = std::true_type{};

	template <typename T, typename... Rest>
	inline constexpr auto is_unique<T, Rest...> = std::bool_constant<
		(!std::is_same_v<T, Rest> && ...) && is_unique<Rest...>
	>{};
#else

template <typename...>
struct is_unique : std::true_type{};

template <typename T0, typename T1, typename... Rest>
struct is_unique<T0, T1, Rest...> : std::integral_constant
<bool,
     !std::is_same<T0, T1>::value 
     && is_unique<T0, Rest...>::value 
     && is_unique<T1, Rest...>::value
>{};

#endif
}

#endif // TWOBLUECUBES_CATCH_TYPE_TRAITS_HPP_INCLUDED
