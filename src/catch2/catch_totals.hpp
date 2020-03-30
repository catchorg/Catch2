/*
 *  Created by Phil Nash on 23/02/2012.
 *  Copyright (c) 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TOTALS_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TOTALS_HPP_INCLUDED

#include <cstddef>

namespace Catch {

    struct Counts {
        Counts operator - ( Counts const& other ) const;
        Counts& operator += ( Counts const& other );

        std::size_t total() const;
        bool allPassed() const;
        bool allOk() const;

        std::size_t passed = 0;
        std::size_t failed = 0;
        std::size_t failedButOk = 0;
    };

    struct Totals {

        Totals operator - ( Totals const& other ) const;
        Totals& operator += ( Totals const& other );

        Totals delta( Totals const& prevTotals ) const;

        int error = 0;
        Counts assertions;
        Counts testCases;
    };
}

#endif // TWOBLUECUBES_CATCH_TOTALS_HPP_INCLUDED
