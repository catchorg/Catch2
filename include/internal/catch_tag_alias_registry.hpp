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

#include <map>
#include <iostream>

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

    void TagAliasRegistry::add( char const* alias, char const* tag, SourceLineInfo const& lineInfo ) {

        if( !startsWith( alias, "[@" ) || !endsWith( alias, "]" ) ) {
            std::ostringstream oss;
            oss << "error: tag alias, \"" << alias << "\" is not of the form [@alias name].\n" << lineInfo;
            throw std::domain_error( oss.str().c_str() );
        }
        if( !m_registry.insert( std::make_pair( alias, TagAlias( tag, lineInfo ) ) ).second ) {
            std::ostringstream oss;
            oss << "error: tag alias, \"" << alias << "\" already registered.\n"
                << "\tFirst seen at " << find(alias)->lineInfo << "\n"
                << "\tRedefined at " << lineInfo;
            throw std::domain_error( oss.str().c_str() );
        }
    }

    TagAliasRegistry& TagAliasRegistry::get() {
        static TagAliasRegistry instance;
        return instance;

    }

    ITagAliasRegistry::~ITagAliasRegistry() {}
    ITagAliasRegistry const& ITagAliasRegistry::get() { return TagAliasRegistry::get(); }


    RegistrarForTagAliases::RegistrarForTagAliases( char const* alias, char const* tag, SourceLineInfo const& lineInfo ) {
        try {
            TagAliasRegistry::get().add( alias, tag, lineInfo );
        }
        catch( std::exception& ex ) {
            Colour colourGuard( Colour::Red );
            std::cerr << ex.what() << std::endl;
            exit(1);
        }
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TAG_ALIAS_REGISTRY_HPP_INCLUDED
