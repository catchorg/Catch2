
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/internal/catch_string_manip.hpp>
#include <catch2/catch_tostring.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>
#include <catch2/internal/catch_case_insensitive_comparisons.hpp>

#include <regex>

namespace Catch {

    namespace {
        constexpr StringRef caseSensitivitySuffix( CaseSensitive caseSensitivity ) {
            return caseSensitivity == CaseSensitive::Yes
                       ? StringRef{}
                       : " (case insensitive)"_sr;
        }
    } // namespace

namespace Matchers {

    StringMatcherBase::StringMatcherBase( std::string target,
                                          StringRef operation,
                                          CaseSensitive caseSensitivity ):
        m_target( CATCH_MOVE( target ) ),
        m_operation( operation ),
        m_caseSensitivity( caseSensitivity ) {}


    std::string StringMatcherBase::describe() const {
        std::string description;
        description.reserve(5 + m_operation.size() + m_target.size() +
                                    caseSensitivitySuffix(m_caseSensitivity).size());
        description += m_operation;
        description += ": \"";
        description += m_target;
        description += '"';
        description += caseSensitivitySuffix(m_caseSensitivity);
        return description;
    }


    StringEqualsMatcher::StringEqualsMatcher( std::string comparator, CaseSensitive caseSensitivity ):
        StringMatcherBase( CATCH_MOVE( comparator ), "equals"_sr, caseSensitivity ) {}

    bool StringEqualsMatcher::match( std::string const& source ) const {
        if (m_caseSensitivity == CaseSensitive::Yes) {
            return m_target == source;
        }
        if (m_target.size() != source.size()) { return false; }
        Catch::Detail::CaseInsensitiveEqualTo eq;
        return eq( m_target, source );
    }


    StringContainsMatcher::StringContainsMatcher(
        std::string comparator, CaseSensitive caseSensitivity ):
        StringMatcherBase( CATCH_MOVE( comparator ), "contains"_sr, caseSensitivity ) {}

    bool StringContainsMatcher::match( std::string const& source ) const {
        if ( m_caseSensitivity == CaseSensitive::Yes ) {
            return contains( source, m_target );
        }
        if ( source.size() < m_target.size() ) { return false; }
        StringRef as_ref( source );
        // The worst case of this is O(m*n), which is terrible, BUT:
        //  * The average case is much better, the worst case only happens rarely
        //  * We can implement BMH/other better searchers later if it matters
        Catch::Detail::CaseInsensitiveEqualTo eq;
        for (size_t i = 0; i < source.size(); ++i) {
            const auto substr = as_ref.substr( i, m_target.size() );
            bool found = eq( substr, m_target );
            if ( found ) { return true; }
        }
        return false;
    }


    StartsWithMatcher::StartsWithMatcher( std::string comparator,
                                          CaseSensitive caseSensitivity ):
        StringMatcherBase( CATCH_MOVE( comparator ), "starts with"_sr, caseSensitivity ) {}

    bool StartsWithMatcher::match( std::string const& source ) const {
        if ( m_caseSensitivity == CaseSensitive::Yes ) {
            return startsWith( source, m_target );
        }
        if (source.size() < m_target.size()) { return false; }
        Catch::Detail::CaseInsensitiveEqualTo eq;
        return eq(
            StringRef( source ).substr( 0, m_target.size() ), m_target );
    }


    EndsWithMatcher::EndsWithMatcher( std::string comparator,
                                      CaseSensitive caseSensitivity ):
        StringMatcherBase( CATCH_MOVE( comparator ), "ends with"_sr, caseSensitivity ) {}

    bool EndsWithMatcher::match( std::string const& source ) const {
        if ( m_caseSensitivity == CaseSensitive::Yes ) {
            return endsWith( source, m_target );
        }
        if ( source.size() < m_target.size() ) { return false; }
        Catch::Detail::CaseInsensitiveEqualTo eq;
        const size_t start_point = source.size() - m_target.size();
        return eq( StringRef( source ).substr( start_point, m_target.size() ), m_target );
    }



    RegexMatcher::RegexMatcher(std::string regex, CaseSensitive caseSensitivity): m_regex(CATCH_MOVE(regex)), m_caseSensitivity(caseSensitivity) {}

    bool RegexMatcher::match(std::string const& matchee) const {
        auto flags = std::regex::ECMAScript; // ECMAScript is the default syntax option anyway
        if (m_caseSensitivity == CaseSensitive::No) {
            flags |= std::regex::icase;
        }
        auto reg = std::regex(m_regex, flags);
        return std::regex_match(matchee, reg);
    }

    std::string RegexMatcher::describe() const {
        return "matches " + ::Catch::Detail::stringify(m_regex) + ((m_caseSensitivity == CaseSensitive::Yes)? " case sensitively" : " case insensitively");
    }


    StringEqualsMatcher Equals( std::string str, CaseSensitive caseSensitivity ) {
        return StringEqualsMatcher( CATCH_MOVE( str ), caseSensitivity );
    }
    StringContainsMatcher ContainsSubstring( std::string str, CaseSensitive caseSensitivity ) {
        return StringContainsMatcher( CATCH_MOVE( str ), caseSensitivity );
    }
    EndsWithMatcher EndsWith( std::string str, CaseSensitive caseSensitivity ) {
        return EndsWithMatcher( CATCH_MOVE( str ), caseSensitivity );
    }
    StartsWithMatcher StartsWith( std::string str, CaseSensitive caseSensitivity ) {
        return StartsWithMatcher( CATCH_MOVE( str ), caseSensitivity );
    }

    RegexMatcher Matches(std::string regex, CaseSensitive caseSensitivity) {
        return RegexMatcher( CATCH_MOVE( regex ), caseSensitivity );
    }

} // namespace Matchers
} // namespace Catch
