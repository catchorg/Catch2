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

            template <typename Ret,
                      typename Tup,
                      typename Fun,
                      std::size_t... Idxs>
            struct TupleRuntimeAccessTable {
                template <std::size_t N>
                static constexpr Ret access_tuple( Tup& tuple, Fun& fun ) {
                    return fun( std::get<N>( tuple ) );
                }

                using AccessorFunPtr = Ret ( * )( Tup&, Fun& );
                static constexpr std::size_t table_size{ sizeof...( Idxs ) };

                static constexpr std::array<AccessorFunPtr, table_size>
                    lookup_table{ { access_tuple<Idxs>... } };
            };

            template <typename Tup, typename Fun, std::size_t... Idxs>
            constexpr auto
            call_access_function( Tup& tuple,
                                  std::size_t index,
                                  Fun fun,
                                  std::index_sequence<Idxs...> ) {
                using FirstTupleIndexType = decltype( std::get<0>( tuple ) );
                using FunReturnType =
                    std::invoke_result_t<Fun, FirstTupleIndexType>;

                constexpr auto& table{
                    TupleRuntimeAccessTable<FunReturnType, Tup, Fun, Idxs...>::
                        lookup_table };
                return table[index]( tuple, fun );
            }

            template <typename Tup, typename Fun>
            constexpr auto
            runtime_get( Tup& tuple, std::size_t index, Fun fun ) {
                return call_access_function(
                    tuple,
                    index,
                    CATCH_FORWARD( fun ),
                    std::make_index_sequence<std::tuple_size_v<Tup>>{} );
            }

            template <typename Tup>
            class TupleAccessor {
                Tup m_tuple;
                std::size_t m_current_index;

            public:
                template <typename... Args>
                constexpr TupleAccessor( Args&&... args ):
                    m_tuple{ CATCH_FORWARD( args )... }, m_current_index{ 0 } {}

                constexpr TupleAccessor& operator++() {
                    ++m_current_index;
                    return *this;
                }

                constexpr operator bool() const {
                    return m_current_index < std::tuple_size_v<Tup>;
                }

                template <typename Fun>
                constexpr auto perform( Fun fun ) const {
                    return runtime_get(
                        m_tuple, m_current_index, CATCH_FORWARD( fun ) );
                }
            };

        } // namespace Detail

        template <typename Tup>
        class TupleGenerator final
            : public IGenerator<Detail::TupleAccessor<Tup>> {
            Detail::TupleAccessor<Tup> m_iterator;

        public:
            template <typename... Args>
            TupleGenerator( Args&&... args ):
                m_iterator{ CATCH_FORWARD( args )... } {}

            const Detail::TupleAccessor<Tup>& get() const override {
                return m_iterator;
            }

            bool next() override { return ++m_iterator; }
        };

        template <typename Tup, typename... Args>
        auto tuple_as_range( Args&&... args ) {
            return GeneratorWrapper<Detail::TupleAccessor<Tup>>(
                Catch::Detail::make_unique<TupleGenerator<Tup>>(
                    CATCH_FORWARD( args )... ) );
        }

        template <typename... Args>
        auto tuple_as_range( Args&&... args ) {
            return tuple_as_range<std::tuple<Args...>>(
                CATCH_FORWARD( args )... );
        }

    } // namespace Generators
} // namespace Catch

#endif // CATCH_GENERATORS_TUPLE_HPINCLUDED
