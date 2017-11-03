/*
 *  Created by Martin on 29/08/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "catch_debug_console.h"
#include "catch_stream.h"
#include "catch_platform.h"

#ifdef CATCH_PLATFORM_WINDOWS

#include "catch_windows_h_proxy.h"

    namespace Catch {
        void writeToDebugConsole( std::string const& text ) {
            ::OutputDebugStringA( text.c_str() );
        }
    }
#else
    namespace Catch {
        void writeToDebugConsole( std::string const& text ) {
            // !TBD: Need a version for Mac/ XCode and other IDEs
            Catch::cout() << text;
        }
    }
#endif // Platform
