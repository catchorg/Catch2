/*
 *  Created by Phil on 27/6/2014.
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TAG_ALIAS_REGISTRY_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TAG_ALIAS_REGISTRY_HPP_INCLUDED

#include "catch_tag_alias_registry.h"
#include "catch_console_colour.hpp"
#include "catch_interfaces_registry_hub.h"
#include "catch_stream.h"

namespace Catch {

    TagAliasRegistry::~TagAliasRegistry() {}

    Option<TagAlias> TagAliasRegistry::find( std::string const& alias ) const {
        std::map<std::string, TagAlias>::const_iterator it = m_registry.find( alias );
        if( it != m_registry.end() )
            return it->second;
        else
            return Option<TagAlias>();
    }

    std::string TagAliasRegistry::expandAliases( std::string const& unexpandedTestSpec ) const {
        std::string expandedTestSpec = unexpandedTestSpec;
        for( auto const& registryKvp : m_registry ) {
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
        // Do not throw when constructing global objects, instead register the exception to be processed later
        if (!(startsWith( alias, "[@") && endsWith(alias, ']'))) {
            getMutableRegistryHub().registerStartupException(
                std::make_exception_ptr(
                    CATCH_PREPARE_EXCEPTION( std::domain_error,
                        "error: tag alias, '"
                        << alias
                        << "' is not of the form [@alias name].\n"
                        << lineInfo)
                )
            );
        }

        if (!m_registry.insert(std::make_pair(alias, TagAlias(tag, lineInfo))).second) {
            getMutableRegistryHub().registerStartupException(
                std::make_exception_ptr(
                    CATCH_PREPARE_EXCEPTION(std::domain_error,
                        "error: tag alias, '" << alias << "' already registered.\n"
                        << "\tFirst seen at: " << find(alias)->lineInfo << "\n"
                        << "\tRedefined at: " << lineInfo)
                )
            );
        }
    }

    ITagAliasRegistry::~ITagAliasRegistry() {}

    ITagAliasRegistry const& ITagAliasRegistry::get() {
        return getRegistryHub().getTagAliasRegistry();
    }

    RegistrarForTagAliases::RegistrarForTagAliases( char const* alias, char const* tag, SourceLineInfo const& lineInfo ) {
        getMutableRegistryHub().registerTagAlias( alias, tag, lineInfo );
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TAG_ALIAS_REGISTRY_HPP_INCLUDED
