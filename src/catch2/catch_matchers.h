/*
 *  Created by Phil Nash on 04/03/2012.
 *  Copyright (c) 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_MATCHERS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_MATCHERS_HPP_INCLUDED

#include <catch2/catch_common.h>

#include <string>
#include <tuple>

namespace Catch {
namespace Matchers {
    namespace Impl {

        template<typename... Matchers> struct MatchAllOf;
        template<typename... Matchers> struct MatchAnyOf;
        template<typename Matcher> struct MatchNotOf;

        template<typename Derived>
        class MatcherBaseGeneric {
        public:
            MatcherBaseGeneric() = default;
            MatcherBaseGeneric ( MatcherBaseGeneric const& ) = default;
            MatcherBaseGeneric& operator = ( MatcherBaseGeneric const& ) = delete;
            std::string toString() const;

            template<typename Other>
            MatchAllOf<Derived, Other> operator && ( Other const& other ) const;

            template<typename Other>
            MatchAnyOf<Derived, Other> operator || ( Other const& other ) const;

            MatchNotOf<Derived> operator ! () const;

            Derived const& derived() const { return static_cast<const Derived &>(*this); }

        protected:
            mutable std::string m_cachedToString;
        };

        template<typename Derived>
        std::string MatcherBaseGeneric<Derived>::toString() const {
            if( m_cachedToString.empty() )
                m_cachedToString = derived().describe();
            return m_cachedToString;
        }

        template<typename... Matchers>
        struct MatchAllOf : MatcherBaseGeneric<MatchAllOf<Matchers...>> {
            MatchAllOf( Matchers const &... matchers ) : m_matchers{matchers...}
            {}

            template<typename ArgT>
            bool match_helper( ArgT && ) const { 
              return true;
            }

            template<typename ArgT, typename Matcher, typename... Rest>
            bool match_helper( ArgT && arg, Matcher &&matcher, Rest &&... rest ) const {
              if (!matcher.match(std::forward<ArgT>(arg))) {
                return false;
              }

              return match_helper(std::forward<ArgT>(arg), std::forward<Rest>(rest)...);
            }

            template<typename ArgT, size_t... Is>
            bool match_impl( ArgT && arg, indexSequence<Is...>) const {
              return match_helper(std::forward<ArgT>(arg), std::get<Is>(m_matchers)...);
            }

            template<typename ArgT>
            bool match( ArgT && arg ) const {
                return match_impl(std::forward<ArgT>(arg), makeIndexSequence<sizeof...(Matchers)>{});
            }

            std::string describe_helper() const { return {}; }

            template<typename Matcher, typename... Rest>
            std::string describe_helper( Matcher &&matcher, Rest &&... rest ) const {
              return " and " + matcher.describe() + describe_helper(std::forward<Rest>(rest)...);
            }

            template<size_t... Is>
            std::string describe_impl( indexSequence<Is...>) const {
              // remove first and
              return describe_helper(std::get<Is>(m_matchers)...).substr(5);
            }            
            
            std::string describe() const {
                std::string description;
                description.reserve( 4 + sizeof...(Matchers)*32 );
                description += "( " + describe_impl(makeIndexSequence<sizeof...(Matchers)>{}) + " )";
                return description;
            }

            template<typename Other, size_t... I>
            MatchAllOf<Matchers..., Other> and_helper(Other const &other, indexSequence<I...>) {
                return {std::get<I>(m_matchers)..., other};
            }

            template<typename Other>
            MatchAllOf<Matchers..., Other> operator && ( Other const& other ) {
                return and_helper(other, makeIndexSequence<sizeof...(Matchers)>());
            }

            std::tuple<Matchers...> m_matchers;
        };

        template<typename... Matchers>
        struct MatchAnyOf : MatcherBaseGeneric<MatchAnyOf<Matchers...>> {
            MatchAnyOf( Matchers const &... matchers ) : m_matchers{matchers...}
            {}

            template<typename ArgT>
            bool match_helper( ArgT && ) const { 
              return false;
            }

            template<typename ArgT, typename Matcher, typename... Rest>
            bool match_helper( ArgT && arg, Matcher &&matcher, Rest &&... rest ) const {
              if (matcher.match(std::forward<ArgT>(arg))) {
                return true;
              }

              return match_helper(std::forward<ArgT>(arg), std::forward<Rest>(rest)...);
            }

            template<typename ArgT, size_t... Is>
            bool match_impl( ArgT && arg, indexSequence<Is...>) const {
              return match_helper(std::forward<ArgT>(arg), std::get<Is>(m_matchers)...);
            }

            template<typename ArgT>
            bool match( ArgT && arg ) const {
                return match_impl(std::forward<ArgT>(arg), makeIndexSequence<sizeof...(Matchers)>{});
            }

            std::string describe_helper() const { return {}; }

            template<typename Matcher, typename... Rest>
            std::string describe_helper( Matcher &&matcher, Rest &&... rest ) const {
              return " or " + matcher.describe() + describe_helper(std::forward<Rest>(rest)...);
            }

            template<size_t... Is>
            std::string describe_impl( indexSequence<Is...>) const {
              // remove first or
              return describe_helper(std::get<Is>(m_matchers)...).substr(4);
            }            
            
            std::string describe() const {
                std::string description;
                description.reserve( 4 + sizeof...(Matchers)*32 );
                description += "( " + describe_impl(makeIndexSequence<sizeof...(Matchers)>{}) + " )";
                return description;
            }

            template<typename Other, size_t... I>
            MatchAnyOf<Matchers..., Other> or_helper(Other const &other, indexSequence<I...>) {
                return {std::get<I>(m_matchers)..., other};
            }

            template<typename Other>
            MatchAnyOf<Matchers..., Other> operator || ( Other const& other ) {
                return or_helper(other, makeIndexSequence<sizeof...(Matchers)>());
            }

            std::tuple<Matchers...> m_matchers;
        };

        template<typename Matcher>
        struct MatchNotOf : MatcherBaseGeneric<MatchNotOf<Matcher>> {

            MatchNotOf( Matcher const& underlyingMatcher ) : m_underlyingMatcher( underlyingMatcher ) {}

            template<typename ArgT>
            bool match( ArgT && arg ) const  {
                return !m_underlyingMatcher.match( std::forward<ArgT>(arg) );
            }

            std::string describe() const  {
                return "not " + m_underlyingMatcher.toString();
            }
            Matcher const& m_underlyingMatcher;
        };

        template<typename Derived>
        template<typename Other>
        MatchAllOf<Derived, Other> MatcherBaseGeneric<Derived>::operator && ( Other const& other ) const {
            return {derived(), other};
        }

        template<typename Derived>
        template<typename Other>
        MatchAnyOf<Derived, Other> MatcherBaseGeneric<Derived>::operator || ( Other const& other ) const {
            return {derived(), other};
        }
        
        template<typename Derived>
        MatchNotOf<Derived> MatcherBaseGeneric<Derived>::operator ! () const {
            return {derived()};
        }

        template<typename Derived>
        Derived const& operator ! (MatchNotOf<Derived> const& matcher) {
            return matcher.m_underlyingMatcher;
        }
        

        template<typename ObjectT>
        struct MatcherMethod {
            virtual bool match( ObjectT const& arg ) const = 0;
        };

#if defined(__OBJC__)
        // Hack to fix Catch GH issue #1661. Could use id for generic Object support.
        // use of const for Object pointers is very uncommon and under ARC it causes some kind of signature mismatch that breaks compilation
        template<>
        struct MatcherMethod<NSString*> {
            virtual bool match( NSString* arg ) const = 0;
        };
#endif

        // for backward compatibility
        template<typename T>
        struct MatcherBase : MatcherBaseGeneric<MatcherBase<T>>, MatcherMethod<T> {
            virtual ~MatcherBase() = default;
            virtual std::string describe() const = 0;
        };

    } // namespace Impl

} // namespace Matchers

using namespace Matchers;
using Matchers::Impl::MatcherBaseGeneric;
using Matchers::Impl::MatcherBase;

} // namespace Catch

#endif // TWOBLUECUBES_CATCH_MATCHERS_HPP_INCLUDED
