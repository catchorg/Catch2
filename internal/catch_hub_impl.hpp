/*
 *  catch_hub_impl.hpp
 *  Test
 *
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_HUB_IMPL_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_HUB_IMPL_HPP_INCLUDED

#include "catch_hub.h"
#include "catch_reporter_registry.hpp"a

namespace Catch
{
    inline Hub::Hub()
    : m_reporterRegistry( new ReporterRegistry )
    {
    }
    
    inline IReporterRegistry& Hub::getReporterRegistry()
    {
        return *me().m_reporterRegistry.get();
    }
}

#endif // TWOBLUECUBES_CATCH_HUB_IMPL_HPP_INCLUDED