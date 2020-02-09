#ifndef TWOBLUECUBES_CATCH_MATCHERS_TEMPLATES_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_MATCHERS_TEMPLATES_HPP_INCLUDED

#include <catch2/catch_common.h>
#include <catch2/catch_matchers.h>
#include <catch2/catch_stringref.h>

#include <array>
#include <algorithm>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace Catch {
namespace Matchers {
    namespace Impl {

        template<typename... MatcherTs> struct MatchAllOfGeneric;
        template<typename... MatcherTs> struct MatchAnyOfGeneric;
        template<typename MatcherT> struct MatchNotOfGeneric;

        struct MatcherGenericBase : MatcherUntypedBase {
            virtual ~MatcherGenericBase();
        };

        template<std::size_t N, std::size_t M>
        std::array<void const*, N + M> array_cat(std::array<void const*, N> && lhs, std::array<void const*, M> && rhs) {
            std::array<void const*, N + M> arr{};
            std::copy_n(lhs.begin(), N, arr.begin());
            std::copy_n(rhs.begin(), M, arr.begin() + N);
            return arr;
        }

        template<std::size_t N>
        std::array<void const*, N+1> array_cat(std::array<void const*, N> && lhs, void const* rhs) {
            std::array<void const*, N+1> arr{};
            std::copy_n(lhs.begin(), N, arr.begin());
            arr[N] = rhs;
            return arr;
        }

        template<std::size_t N>
        std::array<void const*, N+1> array_cat(void const* lhs, std::array<void const*, N> && rhs) {
            std::array<void const*, N+1> arr{lhs};
            std::copy_n(rhs.begin(), N, arr.begin() + 1);
            return arr;
        }

        #ifdef CATCH_CPP17_OR_GREATER

        using std::conjunction;

        #else // CATCH_CPP17_OR_GREATER

        template<typename... Cond>
        struct conjunction : std::true_type {};

        template<typename Cond, typename... Rest>
        struct conjunction<Cond, Rest...> : std::integral_constant<bool, Cond::value && conjunction<Rest...>::value> {};

        #endif // CATCH_CPP17_OR_GREATER


        using std::index_sequence;
        using std::index_sequence_for;
        using std::make_index_sequence;


        template<typename T>
        using is_generic_matcher = std::is_base_of<
            Catch::Matchers::Impl::MatcherGenericBase,
            typename std::remove_cv<typename std::remove_reference<T>::type>::type
        >;

        template<typename... Ts>
        using are_generic_matchers = conjunction<is_generic_matcher<Ts>...>;

        template<typename T>
        using is_matcher = std::is_base_of<
            Catch::Matchers::Impl::MatcherUntypedBase,
            typename std::remove_cv<typename std::remove_reference<T>::type>::type
        >;


        template<std::size_t N, typename Arg>
        bool match_all_of(Arg&&, std::array<void const*, N> const&, index_sequence<>) {
            return true;
        }

        template<typename T, typename... MatcherTs, std::size_t N, typename Arg, std::size_t Idx, std::size_t... Indices>
        bool match_all_of(Arg&& arg, std::array<void const*, N> const& matchers, index_sequence<Idx, Indices...>) {
            return static_cast<T const*>(matchers[Idx])->match(arg) && match_all_of<MatcherTs...>(arg, matchers, index_sequence<Indices...>{});
        }


        template<std::size_t N, typename Arg>
        bool match_any_of(Arg&&, std::array<void const*, N> const&, index_sequence<>) {
            return false;
        }

        template<typename T, typename... MatcherTs, std::size_t N, typename Arg, std::size_t Idx, std::size_t... Indices>
        bool match_any_of(Arg&& arg, std::array<void const*, N> const& matchers, index_sequence<Idx, Indices...>) {
            return static_cast<T const*>(matchers[Idx])->match(arg) || match_any_of<MatcherTs...>(arg, matchers, index_sequence<Indices...>{});
        }

        std::string describe_multi_matcher(StringRef combine, std::string const* descriptions_begin, std::string const* descriptions_end);

        template<typename... MatcherTs, std::size_t... Idx>
        std::string describe_multi_matcher(StringRef combine, std::array<void const*, sizeof...(MatcherTs)> const& matchers, index_sequence<Idx...>) {
            std::array<std::string, sizeof...(MatcherTs)> descriptions {{
                static_cast<MatcherTs const*>(matchers[Idx])->toString()...
            }};

            return describe_multi_matcher(combine, descriptions.data(), descriptions.data() + descriptions.size());
        }


        template<typename... MatcherTs>
        struct MatchAllOfGeneric : MatcherGenericBase {
            MatchAllOfGeneric(MatcherTs const&... matchers) : m_matchers{std::addressof(matchers)...} {}
            explicit MatchAllOfGeneric(std::array<void const*, sizeof...(MatcherTs)> matchers) : m_matchers{matchers} {}

            template<typename Arg>
            bool match(Arg&& arg) const {
                return match_all_of<MatcherTs...>(arg, m_matchers, index_sequence_for<MatcherTs...>{});
            }

            std::string describe() const override {
                return describe_multi_matcher<MatcherTs...>(" and "_sr, m_matchers, index_sequence_for<MatcherTs...>{});
            }

            std::array<void const*, sizeof...(MatcherTs)> m_matchers;
        };


        template<typename... MatcherTs>
        struct MatchAnyOfGeneric : MatcherGenericBase {
            MatchAnyOfGeneric(MatcherTs const&... matchers) : m_matchers{std::addressof(matchers)...} {}
            explicit MatchAnyOfGeneric(std::array<void const*, sizeof...(MatcherTs)> matchers) : m_matchers{matchers} {}

            template<typename Arg>
            bool match(Arg&& arg) const {
                return match_any_of<MatcherTs...>(arg, m_matchers, index_sequence_for<MatcherTs...>{});
            }

            std::string describe() const override {
                return describe_multi_matcher<MatcherTs...>(" or "_sr, m_matchers, index_sequence_for<MatcherTs...>{});
            }

            std::array<void const*, sizeof...(MatcherTs)> m_matchers;
        };


        template<typename MatcherT>
        struct MatchNotOfGeneric : MatcherGenericBase {
            explicit MatchNotOfGeneric(MatcherT const& matcher) : m_matcher{matcher} {}

            template<typename Arg>
            bool match(Arg&& arg) const {
                return !m_matcher.match(arg);
            }

            std::string describe() const override {
                return "not " + m_matcher.toString();
            }

            MatcherT const& m_matcher;
        };

        // compose only generic matchers
        template<typename MatcherLHS, typename MatcherRHS>
        typename std::enable_if<are_generic_matchers<MatcherLHS, MatcherRHS>::value, MatchAllOfGeneric<MatcherLHS, MatcherRHS>>::type
        operator && (MatcherLHS const& lhs, MatcherRHS const& rhs) {
            return {lhs, rhs};
        }

        template<typename MatcherLHS, typename MatcherRHS>
        typename std::enable_if<are_generic_matchers<MatcherLHS, MatcherRHS>::value, MatchAnyOfGeneric<MatcherLHS, MatcherRHS>>::type
        operator || (MatcherLHS const& lhs, MatcherRHS const& rhs) {
            return {lhs, rhs};
        }

        template<typename MatcherT>
        typename std::enable_if<is_generic_matcher<MatcherT>::value, MatchNotOfGeneric<MatcherT>>::type
        operator ! (MatcherT const& matcher) {
            return MatchNotOfGeneric<MatcherT>{matcher};
        }


        // compose mixed generic and non-generic matchers
        template<typename MatcherLHS, typename ArgRHS>
        typename std::enable_if<is_generic_matcher<MatcherLHS>::value, MatchAllOfGeneric<MatcherLHS, MatcherBase<ArgRHS>>>::type
        operator && (MatcherLHS const& lhs, MatcherBase<ArgRHS> const& rhs) {
            return {lhs, rhs};
        }

        template<typename ArgLHS, typename MatcherRHS>
        typename std::enable_if<is_generic_matcher<MatcherRHS>::value, MatchAllOfGeneric<MatcherBase<ArgLHS>, MatcherRHS>>::type
        operator && (MatcherBase<ArgLHS> const& lhs, MatcherRHS const& rhs) {
            return {lhs, rhs};
        }

        template<typename MatcherLHS, typename ArgRHS>
        typename std::enable_if<is_generic_matcher<MatcherLHS>::value, MatchAnyOfGeneric<MatcherLHS, MatcherBase<ArgRHS>>>::type
        operator || (MatcherLHS const& lhs, MatcherBase<ArgRHS> const& rhs) {
            return {lhs, rhs};
        }

        template<typename ArgLHS, typename MatcherRHS>
        typename std::enable_if<is_generic_matcher<MatcherRHS>::value, MatchAnyOfGeneric<MatcherBase<ArgLHS>, MatcherRHS>>::type
        operator || (MatcherBase<ArgLHS> const& lhs, MatcherRHS const& rhs) {
            return {lhs, rhs};
        }


        // avoid deep nesting of matcher templates
        template<typename... MatchersLHS, typename... MatchersRHS>
        MatchAllOfGeneric<MatchersLHS..., MatchersRHS...>
        operator && (MatchAllOfGeneric<MatchersLHS...> && lhs, MatchAllOfGeneric<MatchersRHS...> && rhs) {
            return MatchAllOfGeneric<MatchersLHS..., MatchersRHS...>{array_cat(std::move(lhs.m_matchers), std::move(rhs.m_matchers))};
        }

        template<typename... MatchersLHS, typename MatcherRHS>
        typename std::enable_if<is_matcher<MatcherRHS>::value, MatchAllOfGeneric<MatchersLHS..., MatcherRHS>>::type
        operator && (MatchAllOfGeneric<MatchersLHS...> && lhs, MatcherRHS const& rhs) {
            return MatchAllOfGeneric<MatchersLHS..., MatcherRHS>{array_cat(std::move(lhs.m_matchers), static_cast<void const*>(&rhs))};
        }

        template<typename MatcherLHS, typename... MatchersRHS>
        typename std::enable_if<is_matcher<MatcherLHS>::value, MatchAllOfGeneric<MatcherLHS, MatchersRHS...>>::type
        operator && (MatcherLHS const& lhs, MatchAllOfGeneric<MatchersRHS...> && rhs) {
            return MatchAllOfGeneric<MatcherLHS, MatchersRHS...>{array_cat(static_cast<void const*>(&lhs), std::move(rhs.m_matchers))};
        }

        template<typename... MatchersLHS, typename... MatchersRHS>
        MatchAnyOfGeneric<MatchersLHS..., MatchersRHS...>
        operator || (MatchAnyOfGeneric<MatchersLHS...> && lhs, MatchAnyOfGeneric<MatchersRHS...> && rhs) {
            return MatchAnyOfGeneric<MatchersLHS..., MatchersRHS...>{array_cat(std::move(lhs.m_matchers), std::move(rhs.m_matchers))};
        }

        template<typename... MatchersLHS, typename MatcherRHS>
        typename std::enable_if<is_matcher<MatcherRHS>::value, MatchAnyOfGeneric<MatchersLHS..., MatcherRHS>>::type
        operator || (MatchAnyOfGeneric<MatchersLHS...> && lhs, MatcherRHS const& rhs) {
            return MatchAnyOfGeneric<MatchersLHS..., MatcherRHS>{array_cat(std::move(lhs.m_matchers), static_cast<void const*>(&rhs))};
        }

        template<typename MatcherLHS, typename... MatchersRHS>
        typename std::enable_if<is_matcher<MatcherLHS>::value, MatchAnyOfGeneric<MatcherLHS, MatchersRHS...>>::type
        operator || (MatcherLHS const& lhs, MatchAnyOfGeneric<MatchersRHS...> && rhs) {
            return MatchAnyOfGeneric<MatcherLHS, MatchersRHS...>{array_cat(static_cast<void const*>(&lhs), std::move(rhs.m_matchers))};
        }

        template<typename MatcherT>
        MatcherT const& operator ! (MatchNotOfGeneric<MatcherT> const& matcher) {
            return matcher.m_matcher;
        }

    } // namespace Impl

} // namespace Matchers

using namespace Matchers;
using Matchers::Impl::MatcherGenericBase;

} // namespace Catch

#endif //TWOBLUECUBES_CATCH_MATCHERS_TEMPLATES_HPP_INCLUDED
