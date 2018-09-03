/*
 *  Created by Martin on 01/08/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_ENFORCE_H_INCLUDED
#define TWOBLUECUBES_CATCH_ENFORCE_H_INCLUDED

#include "catch_common.h"
#include "catch_compiler_capabilities.h"
#include "catch_stream.h"


#include <stdexcept>

namespace Catch {
#if !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
    template <typename Ex>
    [[noreturn]]
    void throw_exception(Ex const& e) {
        throw e;
    }
#else // ^^ Exceptions are enabled //  Exceptions are disabled vv
    [[noreturn]]
    void throw_exception(std::exception const& e);
#endif
} // namespace Catch;

#define CATCH_PREPARE_EXCEPTION( type, msg ) \
    type( ( Catch::ReusableStringStream() << msg ).str() )
#define CATCH_INTERNAL_ERROR( msg ) \
    Catch::throw_exception(CATCH_PREPARE_EXCEPTION( std::logic_error, CATCH_INTERNAL_LINEINFO << ": Internal Catch error: " << msg))
#define CATCH_ERROR( msg ) \
    Catch::throw_exception(CATCH_PREPARE_EXCEPTION( std::domain_error, msg ))
#define CATCH_RUNTIME_ERROR( msg ) \
    Catch::throw_exception(CATCH_PREPARE_EXCEPTION( std::runtime_error, msg ))
#define CATCH_ENFORCE( condition, msg ) \
    do{ if( !(condition) ) CATCH_ERROR( msg ); } while(false)


#endif // TWOBLUECUBES_CATCH_ENFORCE_H_INCLUDED
