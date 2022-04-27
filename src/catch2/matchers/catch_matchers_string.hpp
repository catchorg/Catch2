
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_MATCHERS_STRING_HPP_INCLUDED
#define CATCH_MATCHERS_STRING_HPP_INCLUDED

#include <catch2/internal/catch_case_sensitive.hpp>
#include <catch2/internal/catch_dll_public.hpp>
#include <catch2/internal/catch_stringref.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <string>

namespace Catch {
namespace Matchers {

    struct CATCH_DLL_PUBLIC CasedString {
        CasedString( std::string const& str, CaseSensitive caseSensitivity );
        std::string adjustString( std::string const& str ) const;
        StringRef caseSensitivitySuffix() const;

        CaseSensitive m_caseSensitivity;
        std::string m_str;
    };

    class CATCH_DLL_PUBLIC StringMatcherBase : public MatcherBase<std::string> {
    protected:
        CasedString m_comparator;
        StringRef m_operation;

    public:
        StringMatcherBase( StringRef operation,
                           CasedString const& comparator );
        std::string describe() const override;
    };

    class CATCH_DLL_PUBLIC StringEqualsMatcher final : public StringMatcherBase {
    public:
        StringEqualsMatcher( CasedString const& comparator );
        bool match( std::string const& source ) const override;
    };
    class CATCH_DLL_PUBLIC StringContainsMatcher final : public StringMatcherBase {
    public:
        StringContainsMatcher( CasedString const& comparator );
        bool match( std::string const& source ) const override;
    };
    class CATCH_DLL_PUBLIC StartsWithMatcher final : public StringMatcherBase {
    public:
        StartsWithMatcher( CasedString const& comparator );
        bool match( std::string const& source ) const override;
    };
    class CATCH_DLL_PUBLIC EndsWithMatcher final : public StringMatcherBase {
    public:
        EndsWithMatcher( CasedString const& comparator );
        bool match( std::string const& source ) const override;
    };

    class CATCH_DLL_PUBLIC RegexMatcher final : public MatcherBase<std::string> {
        std::string m_regex;
        CaseSensitive m_caseSensitivity;

    public:
        RegexMatcher( std::string regex, CaseSensitive caseSensitivity );
        bool match( std::string const& matchee ) const override;
        std::string describe() const override;
    };

    //! Creates matcher that accepts strings that are exactly equal to `str`
    CATCH_DLL_PUBLIC StringEqualsMatcher
    Equals( std::string const& str,
            CaseSensitive caseSensitivity = CaseSensitive::Yes );
    //! Creates matcher that accepts strings that contain `str`
    CATCH_DLL_PUBLIC StringContainsMatcher
    ContainsSubstring( std::string const& str,
                       CaseSensitive caseSensitivity = CaseSensitive::Yes );
    //! Creates matcher that accepts strings that _end_ with `str`
    CATCH_DLL_PUBLIC EndsWithMatcher
    EndsWith( std::string const& str,
              CaseSensitive caseSensitivity = CaseSensitive::Yes );
    //! Creates matcher that accepts strings that _start_ with `str`
    CATCH_DLL_PUBLIC StartsWithMatcher
    StartsWith( std::string const& str,
                CaseSensitive caseSensitivity = CaseSensitive::Yes );
    //! Creates matcher that accepts strings matching `regex`
    CATCH_DLL_PUBLIC RegexMatcher
    Matches( std::string const& regex,
             CaseSensitive caseSensitivity = CaseSensitive::Yes );

} // namespace Matchers
} // namespace Catch

#endif // CATCH_MATCHERS_STRING_HPP_INCLUDED
