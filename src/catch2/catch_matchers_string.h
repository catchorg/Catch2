/*
 *  Created by Phil Nash on 08/02/2017.
 *  Copyright (c) 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_MATCHERS_STRING_H_INCLUDED
#define TWOBLUECUBES_CATCH_MATCHERS_STRING_H_INCLUDED

#include <catch2/catch_matchers.h>

#include <string>

namespace Catch {
namespace Matchers {

    namespace StdString {

        struct CasedString
        {
            CasedString( std::string const& str, CaseSensitive::Choice caseSensitivity );
            std::string adjustString( std::string const& str ) const;
            std::string caseSensitivitySuffix() const;

            CaseSensitive::Choice m_caseSensitivity;
            std::string m_str;
        };

        template<typename Derived>
        struct StringMatcherBase : MatcherBaseGeneric<Derived> {
            StringMatcherBase( std::string const& operation, CasedString const& comparator );
            std::string describe() const ;

            CasedString m_comparator;
            std::string m_operation;
        };


        template<typename Derived>
        StringMatcherBase<Derived>::StringMatcherBase( std::string const& operation, CasedString const& comparator )
        : m_comparator( comparator ),
          m_operation( operation ) {
        }

        template<typename Derived>
        std::string StringMatcherBase<Derived>::describe() const {
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

        struct EqualsMatcher : StringMatcherBase<EqualsMatcher> {
            EqualsMatcher( CasedString const& comparator );
            bool match( std::string const& source ) const ;
        };
        struct ContainsMatcher : StringMatcherBase<ContainsMatcher> {
            ContainsMatcher( CasedString const& comparator );
            bool match( std::string const& source ) const ;
        };
        struct StartsWithMatcher : StringMatcherBase<StartsWithMatcher> {
            StartsWithMatcher( CasedString const& comparator );
            bool match( std::string const& source ) const ;
        };
        struct EndsWithMatcher : StringMatcherBase<EndsWithMatcher> {
            EndsWithMatcher( CasedString const& comparator );
            bool match( std::string const& source ) const ;
        };

        struct RegexMatcher : MatcherBaseGeneric<RegexMatcher> {
            RegexMatcher( std::string regex, CaseSensitive::Choice caseSensitivity );
            bool match( std::string const& matchee ) const ;
            std::string describe() const ;

        private:
            std::string m_regex;
            CaseSensitive::Choice m_caseSensitivity;
        };

    } // namespace StdString`


    // The following functions create the actual matcher objects.
    // This allows the types to be inferred

    StdString::EqualsMatcher Equals( std::string const& str, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes );
    StdString::ContainsMatcher Contains( std::string const& str, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes );
    StdString::EndsWithMatcher EndsWith( std::string const& str, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes );
    StdString::StartsWithMatcher StartsWith( std::string const& str, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes );
    StdString::RegexMatcher Matches( std::string const& regex, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes );

} // namespace Matchers
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_MATCHERS_STRING_H_INCLUDED
