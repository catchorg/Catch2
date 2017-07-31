/*
 *  Created by Phil Nash on 08/02/2017.
 *  Copyright (c) 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_matchers.hpp"

namespace Catch {
namespace Matchers {

    namespace StdString {

        CasedString::CasedString( std::string const& str, CaseSensitive::Choice caseSensitivity )
        :   m_caseSensitivity( caseSensitivity ),
            m_str( adjustString( str ) )
        {}
        std::string CasedString::adjustString( std::string const& str ) const {
            return m_caseSensitivity == CaseSensitive::No
                   ? toLower( str )
                   : str;
        }
        std::string CasedString::caseSensitivitySuffix() const {
            return m_caseSensitivity == CaseSensitive::No
                   ? " (case insensitive)"
                   : std::string();
        }


        StringMatcherBase::StringMatcherBase( std::string const& operation, CasedString const& comparator )
        : m_comparator( comparator ),
          m_operation( operation ) {
        }

        std::string StringMatcherBase::describe() const {
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

        EqualsMatcher::EqualsMatcher( CasedString const& comparator ) : StringMatcherBase( "equals", comparator ) {}

        bool EqualsMatcher::match( std::string const& source ) const {
            return m_comparator.adjustString( source ) == m_comparator.m_str;
        }


        ContainsMatcher::ContainsMatcher( CasedString const& comparator ) : StringMatcherBase( "contains", comparator ) {}

        bool ContainsMatcher::match( std::string const& source ) const {
            return contains( m_comparator.adjustString( source ), m_comparator.m_str );
        }


        StartsWithMatcher::StartsWithMatcher( CasedString const& comparator ) : StringMatcherBase( "starts with", comparator ) {}

        bool StartsWithMatcher::match( std::string const& source ) const {
            return startsWith( m_comparator.adjustString( source ), m_comparator.m_str );
        }


        EndsWithMatcher::EndsWithMatcher( CasedString const& comparator ) : StringMatcherBase( "ends with", comparator ) {}

        bool EndsWithMatcher::match( std::string const& source ) const {
            return endsWith( m_comparator.adjustString( source ), m_comparator.m_str );
        }

    } // namespace StdString


    StdString::EqualsMatcher Equals( std::string const& str, CaseSensitive::Choice caseSensitivity ) {
        return StdString::EqualsMatcher( StdString::CasedString( str, caseSensitivity) );
    }
    StdString::ContainsMatcher Contains( std::string const& str, CaseSensitive::Choice caseSensitivity ) {
        return StdString::ContainsMatcher( StdString::CasedString( str, caseSensitivity) );
    }
    StdString::EndsWithMatcher EndsWith( std::string const& str, CaseSensitive::Choice caseSensitivity ) {
        return StdString::EndsWithMatcher( StdString::CasedString( str, caseSensitivity) );
    }
    StdString::StartsWithMatcher StartsWith( std::string const& str, CaseSensitive::Choice caseSensitivity ) {
        return StdString::StartsWithMatcher( StdString::CasedString( str, caseSensitivity) );
    }

} // namespace Matchers
} // namespace Catch
