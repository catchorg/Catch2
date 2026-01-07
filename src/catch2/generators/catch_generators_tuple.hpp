//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_GENERATORS_TUPLE_HPINCLUDED
#define CATCH_GENERATORS_TUPLE_HPINCLUDED

#include <catch2/generators/catch_generators.hpp>

#include <array>
#include <tuple>
#include <type_traits>

namespace Catch {
    namespace Generators {
        namespace Detail {

            template <typename T, typename = void>
            struct is_tuple_like : std::false_type {};

            template <typename T>
            struct is_tuple_like<
                T,
                std::void_t<decltype( std::tuple_size<T>::value ),
                            decltype( std::get<0>( std::declval<T>() ) )>>
                : std::true_type {};

            template <typename Ret,
                      typename TupleType,
                      typename Fun,
                      std::size_t N>
            constexpr Ret access_tuple( TupleType& tuple, Fun& fun ) {
                return fun( std::get<N>( tuple ) );
            }

            template <typename Ret,
                      typename TupleType,
                      typename Fun,
                      std::size_t... Idxs>
            constexpr auto tuple_runtime_access_table() {
                using AccessorFunPtr = Ret ( * )( TupleType&, Fun& );
                constexpr std::size_t table_size{ sizeof...( Idxs ) };
                return std::array<AccessorFunPtr, table_size>{
                    { access_tuple<Ret, TupleType, Fun, Idxs>... } };
            }

            template <typename TupleType, typename Fun, std::size_t... Idxs>
            constexpr auto
            call_access_function( TupleType& tuple,
                                  std::size_t index,
                                  Fun fun,
                                  std::index_sequence<Idxs...> ) {
                using FirstTupleIndexType = decltype( std::get<0>( tuple ) );
                using FunReturnType =
#ifdef __cpp_lib_is_invocable // C++ >= 17
                    std::invoke_result_t<Fun, FirstTupleIndexType>;
#else
                    std::result_of_t<Fun( FirstTupleIndexType )>;
#endif

                constexpr auto table{ tuple_runtime_access_table<FunReturnType,
                                                                 TupleType,
                                                                 Fun,
                                                                 Idxs...>() };
                return table[index]( tuple, fun );
            }

            template <typename TupleType, typename Fun>
            constexpr auto
            runtime_get( TupleType& tuple, std::size_t index, Fun fun ) {
                return call_access_function(
                    tuple,
                    index,
                    CATCH_FORWARD( fun ),
                    std::make_index_sequence<
                        std::tuple_size<TupleType>::value>{} );
            }

            template <typename TupleType>
            class TupleAccessor {
                TupleType m_tuple;
                std::size_t m_current_index;

            public:
                template <typename TupleLike>
                constexpr TupleAccessor( TupleLike&& tuple ):
                    m_tuple{ CATCH_FORWARD( tuple ) }, m_current_index{ 0 } {}

                template <typename... Args>
                constexpr TupleAccessor( Args&&... args ):
                    m_tuple{ CATCH_FORWARD( args )... }, m_current_index{ 0 } {}

                constexpr bool next() {
                    ++m_current_index;
                    return m_current_index < std::tuple_size<TupleType>::value;
                }

                template <typename Fun>
                constexpr auto perform( Fun fun ) const {
                    return runtime_get(
                        m_tuple, m_current_index, CATCH_FORWARD( fun ) );
                }
            };

        } // namespace Detail

        template <typename TupleType>
        class TupleGenerator final
            : public IGenerator<Detail::TupleAccessor<TupleType>> {
            Detail::TupleAccessor<TupleType> m_iterator;

        public:
            template <typename TupleLike>
            TupleGenerator( TupleLike&& tuple ):
                m_iterator{ CATCH_FORWARD( tuple ) } {}

            template <typename... Args>
            TupleGenerator( Args&&... args ):
                m_iterator{ CATCH_FORWARD( args )... } {}

            const Detail::TupleAccessor<TupleType>& get() const override {
                return m_iterator;
            }

            bool next() override { return m_iterator.next(); }
        };

        template <typename TupleType,
                  typename... Args,
                  typename =
                      std::enable_if_t<Detail::is_tuple_like<TupleType>::value>>
        auto tuple_as_range( Args&&... args ) {
            return GeneratorWrapper<Detail::TupleAccessor<TupleType>>(
                Catch::Detail::make_unique<TupleGenerator<TupleType>>(
                    CATCH_FORWARD( args )... ) );
        }

        template <typename TupleLike,
                  typename TupleType =
                      std::remove_cv_t<std::remove_reference_t<TupleLike>>,
                  typename =
                      std::enable_if_t<Detail::is_tuple_like<TupleType>::value>>
        auto tuple_as_range( TupleLike&& tuple ) {
            return tuple_as_range<TupleType>( CATCH_FORWARD( tuple ) );
        }

        template <typename... Args>
        auto tuple_as_range( Args&&... args ) {
            return tuple_as_range<std::tuple<Args...>>(
                CATCH_FORWARD( args )... );
        }

    } // namespace Generators
} // namespace Catch

#endif // CATCH_GENERATORS_TUPLE_HPINCLUDED
