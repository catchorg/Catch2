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
        Counts() : passed( 0 ), failed( 0 ) {}

        Counts operator - ( Counts const& other ) const {
            Counts diff;
            diff.passed = passed - other.passed;
            diff.failed = failed - other.failed;
            return diff;
        }
        Counts& operator += ( Counts const& other ) {
            passed += other.passed;
            failed += other.failed;
            return *this;
        }

        std::size_t total() const {
            return passed + failed;
        }

        std::size_t passed;
        std::size_t failed;
    };

    struct Totals {

        Totals operator - ( Totals const& other ) const {
            Totals diff;
            diff.assertions = assertions - other.assertions;
            diff.testCases = testCases - other.testCases;
            return diff;
        }

        Totals delta( Totals const& prevTotals ) const {
            Totals diff = *this - prevTotals;
            if( diff.assertions.failed > 0 )
                ++diff.testCases.failed;
            else
                ++diff.testCases.passed;
            return diff;
        }

        Totals& operator += ( Totals const& other ) {
            assertions += other.assertions;
            testCases += other.testCases;
            return *this;
        }

        Counts assertions;
        Counts testCases;
    };
}

#endif // TWOBLUECUBES_CATCH_TOTALS_HPP_INCLUDED
