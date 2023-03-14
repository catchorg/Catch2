
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_tag_alias_registry.hpp>
#include <catch2/internal/catch_console_colour.hpp>
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/interfaces/catch_interfaces_registry_hub.hpp>
#include <catch2/internal/catch_string_manip.hpp>

#include <map>

namespace Catch {
    struct TagAliasRegistry::TagAliasRegistryImpl {
        std::map<std::string, TagAlias> registry;
    };


    TagAliasRegistry::TagAliasRegistry():
        m_impl( Detail::make_unique<TagAliasRegistryImpl>() ){}
    TagAliasRegistry::~TagAliasRegistry() = default;

    TagAlias const* TagAliasRegistry::find( std::string const& alias ) const {
        auto it = m_impl->registry.find( alias );
        if( it != m_impl->registry.end() )
            return &(it->second);
        else
            return nullptr;
    }

    std::string TagAliasRegistry::expandAliases( std::string const& unexpandedTestSpec ) const {
        std::string expandedTestSpec = unexpandedTestSpec;
        for( auto const& registryKvp : m_impl->registry ) {
            std::size_t pos = expandedTestSpec.find( registryKvp.first );
            if( pos != std::string::npos ) {
                expandedTestSpec =  expandedTestSpec.substr( 0, pos ) +
                                    registryKvp.second.tag +
                                    expandedTestSpec.substr( pos + registryKvp.first.size() );
            }
        }
        return expandedTestSpec;
    }

    void TagAliasRegistry::add( std::string const& alias, std::string const& tag, SourceLineInfo const& lineInfo ) {
        CATCH_ENFORCE( startsWith(alias, "[@") && endsWith(alias, ']'),
                      "error: tag alias, '" << alias << "' is not of the form [@alias name].\n" << lineInfo );

        CATCH_ENFORCE( m_impl->registry.insert(std::make_pair(alias, TagAlias(tag, lineInfo))).second,
                      "error: tag alias, '" << alias << "' already registered.\n"
                      << "\tFirst seen at: " << find(alias)->lineInfo << "\n"
                      << "\tRedefined at: " << lineInfo );
    }

    TagAliasRegistry const& TagAliasRegistry::get() {
        return getRegistryHub().getTagAliasRegistry();
    }

} // end namespace Catch
