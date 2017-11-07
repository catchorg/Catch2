/*
 *  Created by Martin on 01/08/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_ENFORCE_H_INCLUDED
#define TWOBLUECUBES_CATCH_ENFORCE_H_INCLUDED

#include "catch_common.h"
#include "catch_stream.h"

#include <stdexcept>
#include <iosfwd>

#define CATCH_PREPARE_EXCEPTION( type, msg ) \
    type( static_cast<std::ostringstream&&>( Catch::ReusableStringStream().get() << msg ).str() )
#define CATCH_INTERNAL_ERROR( msg ) \
    throw CATCH_PREPARE_EXCEPTION( std::logic_error, CATCH_INTERNAL_LINEINFO << ": Internal Catch error: " << msg);
#define CATCH_ERROR( msg ) \
    throw CATCH_PREPARE_EXCEPTION( std::domain_error, msg )
#define CATCH_ENFORCE( condition, msg ) \
    do{ if( !(condition) ) CATCH_ERROR( msg ); } while(false)

#endif // TWOBLUECUBES_CATCH_ENFORCE_H_INCLUDED
