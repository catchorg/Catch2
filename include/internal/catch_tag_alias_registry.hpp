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
        for( std::map<std::string, TagAlias>::const_iterator it = m_registry.begin(), itEnd = m_registry.end();
                it != itEnd;
                ++it ) {
            std::size_t pos = expandedTestSpec.find( it->first );
            if( pos != std::string::npos ) {
                expandedTestSpec =  expandedTestSpec.substr( 0, pos ) +
                                    it->second.tag +
                                    expandedTestSpec.substr( pos + it->first.size() );
            }
        }
        return expandedTestSpec;
    }

    void TagAliasRegistry::add( std::string const& alias, std::string const& tag, SourceLineInfo const& lineInfo ) {

        if( !startsWith( alias, "[@" ) || !endsWith( alias, ']' ) ) {
            std::ostringstream oss;
            oss << Colour( Colour::Red )
                << "error: tag alias, \"" << alias << "\" is not of the form [@alias name].\n"
                << Colour( Colour::FileName )
                << lineInfo << '\n';
            throw std::domain_error( oss.str().c_str() );
        }
        if( !m_registry.insert( std::make_pair( alias, TagAlias( tag, lineInfo ) ) ).second ) {
            std::ostringstream oss;
            oss << Colour( Colour::Red )
                << "error: tag alias, \"" << alias << "\" already registered.\n"
                << "\tFirst seen at "
                << Colour( Colour::Red ) << find(alias)->lineInfo << '\n'
                << Colour( Colour::Red ) << "\tRedefined at "
                << Colour( Colour::FileName) << lineInfo << '\n';
            throw std::domain_error( oss.str().c_str() );
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
