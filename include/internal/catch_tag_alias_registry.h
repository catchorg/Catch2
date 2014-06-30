/*
 *  Created by Phil on 27/6/2014.
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TAG_ALIAS_REGISTRY_H_INCLUDED
#define TWOBLUECUBES_CATCH_TAG_ALIAS_REGISTRY_H_INCLUDED

#include "catch_interfaces_tag_alias_registry.h"

#include <map>

namespace Catch {

    class TagAliasRegistry : public ITagAliasRegistry {
    public:
        virtual ~TagAliasRegistry();
        virtual Option<TagAlias> find( std::string const& alias ) const;
        virtual std::string expandAliases( std::string const& unexpandedTestSpec ) const;
        void add( char const* alias, char const* tag, SourceLineInfo const& lineInfo );
        static TagAliasRegistry& get();

    private:
        std::map<std::string, TagAlias> m_registry;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_TAG_ALIAS_REGISTRY_H_INCLUDED
