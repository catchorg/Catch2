#ifndef TWOBLUECUBES_CATCH_MATCHERS_TEMPLATES_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_MATCHERS_TEMPLATES_HPP_INCLUDED

#include <catch2/catch_common.h>
#include <catch2/catch_matchers.h>

#include <string>
#include <vector>
#include <type_traits>

namespace Catch {
namespace Matchers {
    namespace Impl {

        template<typename... MatcherTs> struct MatchAllOfGeneric;
        template<typename... MatcherTs> struct MatchAnyOfGeneric;
        template<typename MatcherT> struct MatchNotOfGeneric;

        struct MatcherGenericBase : MatcherUntypedBase {};

        std::vector<void const*> vector_cat(std::vector<void const*> const & lhs, std::vector<void const*> const& rhs);
        std::vector<void const*> vector_cat(std::vector<void const*> const & lhs, void const* rhs);
        std::vector<void const*> vector_cat(void const* lhs, std::vector<void const*> const& rhs);


        #ifdef CATCH_CPP17_OR_GREATER

        using std::conjunction;

        #else // CATCH_CPP17_OR_GREATER

        template<typename... Cond>
        struct conjunction : std::true_type {};

        template<typename Cond, typename... Rest>
        struct conjunction<Cond, Rest...> : std::integral_constant<bool, Cond::value && conjunction<Rest...>::value> {};

        #endif // CATCH_CPP17_OR_GREATER


        #ifdef CATCH_CPP14_OR_GREATER

        using std::index_sequence;
        using std::index_sequence_for;
        using std::make_index_sequence;

        #else // CATCH_CPP14_OR_GREATER

        /* C++14 index_sequence for C++11 */

        template<typename T, T... Ints>
        struct integer_sequence {
            using value_type = T;
            static constexpr std::size_t size() { return sizeof...(Ints); }
        };

        template<std::size_t... Ints>
        using index_sequence = integer_sequence<std::size_t, Ints...>;

        template<typename T, std::size_t N, T... Is>
        struct make_integer_sequence : make_integer_sequence<T, N-1, N-1, Is...> {};

        template<typename T, T... Is>
        struct make_integer_sequence<T, 0, Is...> : integer_sequence<T, Is...> {};

        template<std::size_t N>
        using make_index_sequence = make_integer_sequence<std::size_t, N>;

        template<typename... T>
        using index_sequence_for = make_index_sequence<sizeof...(T)>;

        #endif // CATCH_CPP14_OR_GREATER


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


        template<typename..., typename Arg>
        bool match_all_of(Arg const&, std::vector<void const*> const&, index_sequence<>) {
            return true;
        }

        template<typename T, typename... MatcherTs, typename Arg, std::size_t Idx, std::size_t... Indices>
        bool match_all_of(Arg const& arg, std::vector<void const*> const& matchers, index_sequence<Idx, Indices...>) {
            return static_cast<T const*>(matchers[Idx])->match(arg) && match_all_of<MatcherTs...>(arg, matchers, index_sequence<Indices...>{});
        }


        template<typename..., typename Arg>
        bool match_any_of(Arg const&, std::vector<void const*> const&, index_sequence<>) {
            return false;
        }

        template<typename T, typename... MatcherTs, typename Arg, std::size_t Idx, std::size_t... Indices>
        bool match_any_of(Arg const& arg, std::vector<void const*> const& matchers, index_sequence<Idx, Indices...>) {
            return static_cast<T const*>(matchers[Idx])->match(arg) || match_any_of<MatcherTs...>(arg, matchers, index_sequence<Indices...>{});
        }


        template<typename... MatcherTs, std::size_t... Idx>
        std::string describe_multi_matcher(std::string const& combine, std::vector<void const*> const& matchers, index_sequence<Idx...>) {
            std::array<std::string, sizeof...(MatcherTs)> descriptions {
                static_cast<MatcherTs const*>(matchers[Idx])->toString()...
            };

            std::string description;
            description.reserve( 4 + descriptions.size() * 32 );
            description += "( ";
            bool first = true;
            for( auto const& desc : descriptions ) {
                if( first )
                    first = false;
                else
                    description += combine;
                description += desc;
            }
            description += " )";
            return description;
        }


        template<typename... MatcherTs>
        struct MatchAllOfGeneric : MatcherGenericBase {
            MatchAllOfGeneric(MatcherTs const&... matchers) : m_matchers{&matchers...} {}
            explicit MatchAllOfGeneric(std::vector<void const*> && matchers) : m_matchers{std::move(matchers)} {}

            template<typename Arg>
            bool match(const Arg& arg) const {
                return match_all_of<MatcherTs...>(arg, m_matchers, index_sequence_for<MatcherTs...>{});
            }

            std::string describe() const override {
                return describe_multi_matcher<MatcherTs...>(" and ", m_matchers, index_sequence_for<MatcherTs...>{});
            }

            std::vector<void const*> m_matchers;
        };


        template<typename... MatcherTs>
        struct MatchAnyOfGeneric : MatcherGenericBase {
            MatchAnyOfGeneric(MatcherTs const&... matchers) : m_matchers{&matchers...} {}
            explicit MatchAnyOfGeneric(std::vector<void const*> && matchers) : m_matchers{std::move(matchers)} {}

            template<typename Arg>
            bool match(const Arg& arg) const {
                return match_any_of<MatcherTs...>(arg, m_matchers, index_sequence_for<MatcherTs...>{});
            }

            std::string describe() const override {
                return describe_multi_matcher<MatcherTs...>(" or ", m_matchers, index_sequence_for<MatcherTs...>{});
            }

            std::vector<void const*> m_matchers;
        };


        template<typename MatcherT>
        struct MatchNotOfGeneric : MatcherGenericBase {
            explicit MatchNotOfGeneric(MatcherT const& matcher) : m_matcher{matcher} {}

            template<typename Arg>
            bool match(const Arg& arg) const {
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
        operator && (MatchAllOfGeneric<MatchersLHS...> const& lhs, MatchAllOfGeneric<MatchersRHS...> const& rhs) {
            return MatchAllOfGeneric<MatchersLHS..., MatchersRHS...>{vector_cat(lhs.m_matchers, rhs.m_matchers)};
        }

        template<typename... MatchersLHS, typename MatcherRHS>
        typename std::enable_if<is_matcher<MatcherRHS>::value, MatchAllOfGeneric<MatchersLHS..., MatcherRHS>>::type
        operator && (MatchAllOfGeneric<MatchersLHS...> const& lhs, MatcherRHS const& rhs) {
            return MatchAllOfGeneric<MatchersLHS..., MatcherRHS>{vector_cat(lhs.m_matchers, static_cast<void const*>(&rhs))};
        }

        template<typename MatcherLHS, typename... MatchersRHS>
        typename std::enable_if<is_matcher<MatcherLHS>::value, MatchAllOfGeneric<MatcherLHS, MatchersRHS...>>::type
        operator && (MatcherLHS const& lhs, MatchAllOfGeneric<MatchersRHS...> const& rhs) {
            return MatchAllOfGeneric<MatcherLHS, MatchersRHS...>{vector_cat(static_cast<void const*>(&lhs), rhs.m_matchers)};
        }

        template<typename... MatchersLHS, typename... MatchersRHS>
        MatchAnyOfGeneric<MatchersLHS..., MatchersRHS...>
        operator || (MatchAnyOfGeneric<MatchersLHS...> const& lhs, MatchAnyOfGeneric<MatchersRHS...> const& rhs) {
            return MatchAnyOfGeneric<MatchersLHS..., MatchersRHS...>{vector_cat(lhs.m_matchers, rhs.m_matchers)};
        }

        template<typename... MatchersLHS, typename MatcherRHS>
        typename std::enable_if<is_matcher<MatcherRHS>::value, MatchAnyOfGeneric<MatchersLHS..., MatcherRHS>>::type
        operator || (MatchAnyOfGeneric<MatchersLHS...> const& lhs, MatcherRHS const& rhs) {
            return MatchAnyOfGeneric<MatchersLHS..., MatcherRHS>{vector_cat(lhs.m_matchers, static_cast<void const*>(&rhs))};
        }

        template<typename MatcherLHS, typename... MatchersRHS>
        typename std::enable_if<is_matcher<MatcherLHS>::value, MatchAnyOfGeneric<MatcherLHS, MatchersRHS...>>::type
        operator || (MatcherLHS const& lhs, MatchAnyOfGeneric<MatchersRHS...> const& rhs) {
            return MatchAnyOfGeneric<MatcherLHS, MatchersRHS...>{vector_cat(static_cast<void const*>(&lhs), rhs.m_matchers)};
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
