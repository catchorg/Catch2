/*
 *  Created by Phil on 14/8/2012.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEST_SPEC_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TEST_SPEC_HPP_INCLUDED

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

#include <catch2/internal/catch_wildcard_pattern.hpp>

#include <string>
#include <vector>
#include <memory>

namespace Catch {

    struct IConfig;
    struct TestCaseInfo;
    class TestCaseHandle;

    class TestSpec {

        class Pattern {
        public:
            explicit Pattern( std::string const& name );
            virtual ~Pattern();
            virtual bool matches( TestCaseInfo const& testCase ) const = 0;
            std::string const& name() const;
        private:
            std::string const m_name;
        };

        class NamePattern : public Pattern {
        public:
            explicit NamePattern( std::string const& name, std::string const& filterString );
            bool matches( TestCaseInfo const& testCase ) const override;
        private:
            WildcardPattern m_wildcardPattern;
        };

        class TagPattern : public Pattern {
        public:
            explicit TagPattern( std::string const& tag, std::string const& filterString );
            bool matches( TestCaseInfo const& testCase ) const override;
        private:
            std::string m_tag;
        };

        struct Filter {
            std::vector<std::unique_ptr<Pattern>> m_required;
            std::vector<std::unique_ptr<Pattern>> m_forbidden;

            bool matches( TestCaseInfo const& testCase ) const;
            std::string name() const;
        };

    public:
        struct FilterMatch {
            std::string name;
            std::vector<TestCaseHandle const*> tests;
        };
        using Matches = std::vector<FilterMatch>;
        using vectorStrings = std::vector<std::string>;

        bool hasFilters() const;
        bool matches( TestCaseInfo const& testCase ) const;
        Matches matchesByFilter( std::vector<TestCaseHandle> const& testCases, IConfig const& config ) const;
        const vectorStrings & getInvalidArgs() const;

    private:
        std::vector<Filter> m_filters;
        std::vector<std::string> m_invalidArgs;
        friend class TestSpecParser;
    };
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_CATCH_TEST_SPEC_HPP_INCLUDED
