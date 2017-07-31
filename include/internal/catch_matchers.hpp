/*
 *  Created by Phil Nash on 04/03/2012.
 *  Copyright (c) 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_MATCHERS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_MATCHERS_HPP_INCLUDED

#include "catch_common.h"

namespace Catch {
namespace Matchers {
    namespace Impl {

        template<typename ArgT> struct MatchAllOf;
        template<typename ArgT> struct MatchAnyOf;
        template<typename ArgT> struct MatchNotOf;

        class MatcherUntypedBase {
        public:
            std::string toString() const {
                if( m_cachedToString.empty() )
                    m_cachedToString = describe();
                return m_cachedToString;
            }

        protected:
            virtual ~MatcherUntypedBase();
            virtual std::string describe() const = 0;
            mutable std::string m_cachedToString;
        private:
            MatcherUntypedBase& operator = ( MatcherUntypedBase const& );
        };

        template<typename ObjectT>
        struct MatcherMethod {
            virtual bool match( ObjectT const& arg ) const = 0;
        };
        template<typename PtrT>
        struct MatcherMethod<PtrT*> {
            virtual bool match( PtrT* arg ) const = 0;
        };

        template<typename ObjectT, typename ComparatorT = ObjectT>
        struct MatcherBase : MatcherUntypedBase, MatcherMethod<ObjectT> {


            MatchAllOf<ComparatorT> operator && ( MatcherBase const& other ) const;
            MatchAnyOf<ComparatorT> operator || ( MatcherBase const& other ) const;
            MatchNotOf<ComparatorT> operator ! () const;
        };

        template<typename ArgT>
        struct MatchAllOf : MatcherBase<ArgT> {
            virtual bool match( ArgT const& arg ) const CATCH_OVERRIDE {
                for( std::size_t i = 0; i < m_matchers.size(); ++i ) {
                    if (!m_matchers[i]->match(arg))
                        return false;
                }
                return true;
            }
            virtual std::string describe() const CATCH_OVERRIDE {
                std::string description;
                description.reserve( 4 + m_matchers.size()*32 );
                description += "( ";
                for( std::size_t i = 0; i < m_matchers.size(); ++i ) {
                    if( i != 0 )
                        description += " and ";
                    description += m_matchers[i]->toString();
                }
                description += " )";
                return description;
            }

            MatchAllOf<ArgT>& operator && ( MatcherBase<ArgT> const& other ) {
                m_matchers.push_back( &other );
                return *this;
            }

            std::vector<MatcherBase<ArgT> const*> m_matchers;
        };
        template<typename ArgT>
        struct MatchAnyOf : MatcherBase<ArgT> {

            virtual bool match( ArgT const& arg ) const CATCH_OVERRIDE {
                for( std::size_t i = 0; i < m_matchers.size(); ++i ) {
                    if (m_matchers[i]->match(arg))
                        return true;
                }
                return false;
            }
            virtual std::string describe() const CATCH_OVERRIDE {
                std::string description;
                description.reserve( 4 + m_matchers.size()*32 );
                description += "( ";
                for( std::size_t i = 0; i < m_matchers.size(); ++i ) {
                    if( i != 0 )
                        description += " or ";
                    description += m_matchers[i]->toString();
                }
                description += " )";
                return description;
            }

            MatchAnyOf<ArgT>& operator || ( MatcherBase<ArgT> const& other ) {
                m_matchers.push_back( &other );
                return *this;
            }

            std::vector<MatcherBase<ArgT> const*> m_matchers;
        };

        template<typename ArgT>
        struct MatchNotOf : MatcherBase<ArgT> {

            MatchNotOf( MatcherBase<ArgT> const& underlyingMatcher ) : m_underlyingMatcher( underlyingMatcher ) {}

            virtual bool match( ArgT const& arg ) const CATCH_OVERRIDE {
                return !m_underlyingMatcher.match( arg );
            }

            virtual std::string describe() const CATCH_OVERRIDE {
                return "not " + m_underlyingMatcher.toString();
            }
            MatcherBase<ArgT> const& m_underlyingMatcher;
        };

        template<typename ObjectT, typename ComparatorT>
        MatchAllOf<ComparatorT> MatcherBase<ObjectT, ComparatorT>::operator && ( MatcherBase const& other ) const {
            return MatchAllOf<ComparatorT>() && *this && other;
        }
        template<typename ObjectT, typename ComparatorT>
        MatchAnyOf<ComparatorT> MatcherBase<ObjectT, ComparatorT>::operator || ( MatcherBase const& other ) const {
            return MatchAnyOf<ComparatorT>() || *this || other;
        }
        template<typename ObjectT, typename ComparatorT>
        MatchNotOf<ComparatorT> MatcherBase<ObjectT, ComparatorT>::operator ! () const {
            return MatchNotOf<ComparatorT>( *this );
        }

    } // namespace Impl


    // The following functions create the actual matcher objects.
    // This allows the types to be inferred
    // - deprecated: prefer ||, && and !
    template<typename T>
    Impl::MatchNotOf<T> Not( Impl::MatcherBase<T> const& underlyingMatcher ) {
        return Impl::MatchNotOf<T>( underlyingMatcher );
    }
    template<typename T>
    Impl::MatchAllOf<T> AllOf( Impl::MatcherBase<T> const& m1, Impl::MatcherBase<T> const& m2 ) {
        return Impl::MatchAllOf<T>() && m1 && m2;
    }
    template<typename T>
    Impl::MatchAllOf<T> AllOf( Impl::MatcherBase<T> const& m1, Impl::MatcherBase<T> const& m2, Impl::MatcherBase<T> const& m3 ) {
        return Impl::MatchAllOf<T>() && m1 && m2 && m3;
    }
    template<typename T>
    Impl::MatchAnyOf<T> AnyOf( Impl::MatcherBase<T> const& m1, Impl::MatcherBase<T> const& m2 ) {
        return Impl::MatchAnyOf<T>() || m1 || m2;
    }
    template<typename T>
    Impl::MatchAnyOf<T> AnyOf( Impl::MatcherBase<T> const& m1, Impl::MatcherBase<T> const& m2, Impl::MatcherBase<T> const& m3 ) {
        return Impl::MatchAnyOf<T>() || m1 || m2 || m3;
    }

} // namespace Matchers

using namespace Matchers;
using Matchers::Impl::MatcherBase;

} // namespace Catch

#endif // TWOBLUECUBES_CATCH_MATCHERS_HPP_INCLUDED
