/*
 *  Created by Martin on 30/08/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RANDOM_NUMBER_GENERATOR_H_INCLUDED
#define TWOBLUECUBES_CATCH_RANDOM_NUMBER_GENERATOR_H_INCLUDED

#include <algorithm>
#include <random>

namespace Catch {

    struct IConfig;

    unsigned int rngSeed();

    class RandomNumberGenerator {
    public:
        using result_type = unsigned int;
        
        RandomNumberGenerator( int seed = rngSeed() );

        static constexpr result_type (min)() { return 0; }
        static constexpr result_type (max)() { return 1000000; }

        result_type operator()( result_type n );
        result_type operator()();

        template<typename V>
        static void shuffle( V& vector ) {
            RandomNumberGenerator rng;
            std::shuffle( vector.begin(), vector.end(), rng );
        }

    private:
        std::default_random_engine generator;
    };

}

#endif // TWOBLUECUBES_CATCH_RANDOM_NUMBER_GENERATOR_H_INCLUDED
