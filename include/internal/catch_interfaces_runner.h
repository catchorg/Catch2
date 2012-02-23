/*
 *  catch_interfaces_runner.h
 *  Catch
 *
 *  Created by Phil on 07/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_INTERNAL_CATCH_INTERFACES_RUNNER_H_INCLUDED
#define TWOBLUECUBES_INTERNAL_CATCH_INTERFACES_RUNNER_H_INCLUDED

#include "catch_totals.hpp"

#include <string>

namespace Catch
{
    class TestCaseInfo;
    
    struct IRunner
    {
        virtual ~IRunner
        ()
        {}
        
        virtual void runAll
            (   bool runHiddenTests = false
            ) = 0;
        
        virtual std::size_t runMatching
            ( const std::string& rawTestSpec 
            ) = 0;
        
        virtual Totals getTotals
            () const = 0;
        
    };
}

#endif // TWOBLUECUBES_INTERNAL_CATCH_INTERFACES_RUNNER_H_INCLUDED
