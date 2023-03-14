
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_TAG_ALIAS_REGISTRY_HPP_INCLUDED
#define CATCH_TAG_ALIAS_REGISTRY_HPP_INCLUDED

#include <catch2/catch_tag_alias.hpp>
#include <catch2/internal/catch_unique_ptr.hpp>

#include <string>

namespace Catch {
    struct SourceLineInfo;

    class TagAliasRegistry {
        struct TagAliasRegistryImpl;
        Detail::unique_ptr<TagAliasRegistryImpl> m_impl;
    public:
        TagAliasRegistry();
        ~TagAliasRegistry(); // = default;

        //! Nullptr if not present
        TagAlias const* find( std::string const& alias ) const;
        //! Returns the test spec but with expanded aliases
        std::string expandAliases( std::string const& unexpandedTestSpec ) const;
        void add( std::string const& alias, std::string const& tag, SourceLineInfo const& lineInfo );

        static TagAliasRegistry const& get();

    private:
    };

} // end namespace Catch

#endif // CATCH_TAG_ALIAS_REGISTRY_HPP_INCLUDED
