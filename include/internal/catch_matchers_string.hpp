/*
 *  Created by Phil Nash on 08/02/2017.
 *  Copyright (c) 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_MATCHERS_STRING_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_MATCHERS_STRING_HPP_INCLUDED

#include "catch_matchers.hpp"

namespace Catch {
namespace Matchers {

    namespace StdString {

        struct CasedString
        {
            CasedString( std::string const& str, CaseSensitive::Choice caseSensitivity )
            :   m_caseSensitivity( caseSensitivity ),
                m_str( adjustString( str ) )
            {}
            std::string adjustString( std::string const& str ) const {
                return m_caseSensitivity == CaseSensitive::No
                       ? toLower( str )
                       : str;
            }
            std::string caseSensitivitySuffix() const {
                return m_caseSensitivity == CaseSensitive::No
                       ? " (case insensitive)"
                       : std::string();
            }
            CaseSensitive::Choice m_caseSensitivity;
            std::string m_str;
        };

        // !TBD Move impl
        struct StringMatcherBase : Impl::MatcherBase<std::string> {
            StringMatcherBase( std::string operation, CasedString const& comparator )
            : m_comparator( comparator ),
              m_operation( operation ) {
            }

            virtual std::string toStringUncached() const CATCH_OVERRIDE {
                std::string description;
                description.reserve(5 + m_operation.size() + m_comparator.m_str.size() +
                                            m_comparator.caseSensitivitySuffix().size());
                description += m_operation;
                description += ": \"";
                description += m_comparator.m_str;
                description += "\"";
                description += m_comparator.caseSensitivitySuffix();
                return description;
            }

            CasedString m_comparator;
            std::string m_operation;
        };

        // !TBD Move impl
        struct EqualsMatcher : StringMatcherBase {
            EqualsMatcher( CasedString const& comparator ) : StringMatcherBase( "equals", comparator ) {}

            virtual bool match( std::string const& source ) const CATCH_OVERRIDE {
                return m_comparator.adjustString( source ) == m_comparator.m_str;
            }
        };
        struct ContainsMatcher : StringMatcherBase {
            ContainsMatcher( CasedString const& comparator ) : StringMatcherBase( "contains", comparator ) {}

            virtual bool match( std::string const& source ) const CATCH_OVERRIDE {
                return contains( m_comparator.adjustString( source ), m_comparator.m_str );
            }
        };
        struct StartsWithMatcher : StringMatcherBase {
            StartsWithMatcher( CasedString const& comparator ) : StringMatcherBase( "starts with", comparator ) {}

            virtual bool match( std::string const& source ) const CATCH_OVERRIDE {
                return startsWith( m_comparator.adjustString( source ), m_comparator.m_str );
            }
        };
        struct EndsWithMatcher : StringMatcherBase {
            EndsWithMatcher( CasedString const& comparator ) : StringMatcherBase( "ends with", comparator ) {}

            virtual bool match( std::string const& source ) const CATCH_OVERRIDE {
                return endsWith( m_comparator.adjustString( source ), m_comparator.m_str );
            }
        };

    } // namespace StdString



    // The following functions create the actual matcher objects.
    // This allows the types to be inferred

    inline StdString::EqualsMatcher Equals( std::string const& str, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes ) {
        return StdString::EqualsMatcher( StdString::CasedString( str, caseSensitivity) );
    }
    inline StdString::ContainsMatcher Contains( std::string const& str, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes ) {
        return StdString::ContainsMatcher( StdString::CasedString( str, caseSensitivity) );
    }
    inline StdString::EndsWithMatcher EndsWith( std::string const& str, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes ) {
        return StdString::EndsWithMatcher( StdString::CasedString( str, caseSensitivity) );
    }
    inline StdString::StartsWithMatcher StartsWith( std::string const& str, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes ) {
        return StdString::StartsWithMatcher( StdString::CasedString( str, caseSensitivity) );
    }

} // namespace Matchers
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_MATCHERS_STRING_HPP_INCLUDED
