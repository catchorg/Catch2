/*
 *  Created by Martin on 04/06/2017.
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_startup_exception_registry.h"

namespace Catch {
    void StartupExceptionRegistry::add( std::exception_ptr const& exception ) {
        m_exceptions.push_back(exception);
    }

    std::vector<std::exception_ptr> const& StartupExceptionRegistry::getExceptions() const {
        return m_exceptions;
    }

} // end namespace Catch
