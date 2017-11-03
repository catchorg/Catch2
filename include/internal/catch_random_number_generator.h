/*
 *  Created by Martin on 30/08/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RANDOM_NUMBER_GENERATOR_H_INCLUDED
#define TWOBLUECUBES_CATCH_RANDOM_NUMBER_GENERATOR_H_INCLUDED

#include <algorithm>

namespace Catch {

    struct IConfig;

    void seedRng( IConfig const& config );

    unsigned int rngSeed();

    struct RandomNumberGenerator {
        using result_type = unsigned int;

        static constexpr result_type (min)() { return 0; }
        static constexpr result_type (max)() { return 1000000; }

        result_type operator()( result_type n ) const;
        result_type operator()() const;

        template<typename V>
        static void shuffle( V& vector ) {
            RandomNumberGenerator rng;
            std::shuffle( vector.begin(), vector.end(), rng );
        }
    };

}

#endif // TWOBLUECUBES_CATCH_RANDOM_NUMBER_GENERATOR_H_INCLUDED
