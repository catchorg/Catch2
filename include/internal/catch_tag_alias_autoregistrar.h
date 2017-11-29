/*
 *  Created by Martin on 27/07/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TAG_ALIAS_AUTOREGISTRAR_H_INCLUDED
#define TWOBLUECUBES_CATCH_TAG_ALIAS_AUTOREGISTRAR_H_INCLUDED

#include "catch_common.h"

namespace Catch {

    struct RegistrarForTagAliases {
        RegistrarForTagAliases( char const* alias, char const* tag, SourceLineInfo const& lineInfo );
    };

} // end namespace Catch

#define CATCH_REGISTER_TAG_ALIAS( alias, spec ) \
    CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
    namespace{ Catch::RegistrarForTagAliases INTERNAL_CATCH_UNIQUE_NAME( AutoRegisterTagAlias )( alias, spec, CATCH_INTERNAL_LINEINFO ); } \
    CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS

#endif // TWOBLUECUBES_CATCH_TAG_ALIAS_AUTOREGISTRAR_H_INCLUDED
