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
template< typename... >
struct TypeList {};

template< typename... >
struct append;

template< template<typename...> class L1
    , typename...E1
    , template<typename...> class L2
    , typename...E2
>
struct append< L1<E1...>, L2<E2...> > {
    using type = L1<E1..., E2...>;
};

template< template<typename...> class L1
    , typename...E1
    , template<typename...> class L2
    , typename...E2
    , typename...Rest
>
struct append< L1<E1...>, L2<E2...>, Rest...> {
    using type = typename append< L1<E1..., E2...>, Rest... >::type;
};

template< template<typename...> class
    , typename...
>
struct rewrap;

template< template<typename...> class Container
    , template<typename...> class List
    , typename...elems
>
struct rewrap<Container, List<elems...>> {
    using type = TypeList< Container< elems... > >;
};

template< template<typename...> class Container
    , template<typename...> class List
    , class...Elems
    , typename...Elements>
    struct rewrap<Container, List<Elems...>, Elements...> {
    using type = typename append<TypeList<Container<Elems...>>, typename rewrap<Container, Elements...>::type>::type;
};

template< template<typename...> class...Containers >
struct combine {
    template< typename...Types >
    struct with_types {
        template< template <typename...> class Final >
        struct into {
            using type = typename append<Final<>, typename rewrap<Containers, Types...>::type...>::type;
        };
    };
};

template<typename T>
struct always_false : std::false_type {};

} // namespace Catch

#endif // TWOBLUECUBES_CATCH_META_HPP_INCLUDED
