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

    std::mt19937& rng();
    void seedRng( IConfig const& config );
    unsigned int rngSeed();

}

#endif // TWOBLUECUBES_CATCH_RANDOM_NUMBER_GENERATOR_H_INCLUDED
