//
//  catch_totals.hpp
//  Catch
//
//  Created by Phil Nash on 23/02/2012.
//  Copyright (c) 2012 Two Blue Cubes Ltd. All rights reserved.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TWOBLUECUBES_CATCH_TOTALS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TOTALS_HPP_INCLUDED

namespace Catch
{
    struct Counts
    {
        Counts
        ()
        :   passed( 0 ), 
            failed( 0 )
        {}

        Counts operator - ( const Counts& other ) const
        {
            Counts diff;
            diff.passed = passed - other.passed;
            diff.failed = failed - other.failed;
            return diff;
        }
        
        std::size_t total() const
        {
            return passed + failed;
        }
        std::size_t passed;
        std::size_t failed;
    };
    
    struct Totals
    {
        Totals operator - ( const Totals& other ) const
        {
            Totals diff;
            diff.assertions = assertions - other.assertions;
            diff.testCases = testCases - other.testCases;
            return diff;
        }
        
        Counts assertions;
        Counts testCases;
    };
}

#endif // TWOBLUECUBES_CATCH_TOTALS_HPP_INCLUDED
