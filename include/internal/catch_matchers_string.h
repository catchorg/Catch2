/*
 *  Created by Phil Nash on 08/02/2017.
 *  Copyright (c) 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_MATCHERS_STRING_H_INCLUDED
#define TWOBLUECUBES_CATCH_MATCHERS_STRING_H_INCLUDED

#include "catch_matchers.hpp"

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

        struct StringMatcherBase : MatcherBase<std::string> {
            StringMatcherBase( std::string const& operation, CasedString const& comparator );
            virtual std::string describe() const CATCH_OVERRIDE;

            CasedString m_comparator;
            std::string m_operation;
        };

        struct EqualsMatcher : StringMatcherBase {
            EqualsMatcher( CasedString const& comparator );
            virtual bool match( std::string const& source ) const CATCH_OVERRIDE;
        };
        struct ContainsMatcher : StringMatcherBase {
            ContainsMatcher( CasedString const& comparator );
            virtual bool match( std::string const& source ) const CATCH_OVERRIDE;
        };
        struct StartsWithMatcher : StringMatcherBase {
            StartsWithMatcher( CasedString const& comparator );
            virtual bool match( std::string const& source ) const CATCH_OVERRIDE;
        };
        struct EndsWithMatcher : StringMatcherBase {
            EndsWithMatcher( CasedString const& comparator );
            virtual bool match( std::string const& source ) const CATCH_OVERRIDE;
        };

    } // namespace StdString


    // The following functions create the actual matcher objects.
    // This allows the types to be inferred

    StdString::EqualsMatcher Equals( std::string const& str, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes );
    StdString::ContainsMatcher Contains( std::string const& str, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes );
    StdString::EndsWithMatcher EndsWith( std::string const& str, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes );
    StdString::StartsWithMatcher StartsWith( std::string const& str, CaseSensitive::Choice caseSensitivity = CaseSensitive::Yes );

} // namespace Matchers
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_MATCHERS_STRING_H_INCLUDED
