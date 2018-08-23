/*
 *  Created by Phil Nash on 15/6/2018.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_generators.hpp"
#include "catch_random_number_generator.h"
#include "catch_interfaces_capture.h"

#include <limits>
#include <set>

namespace Catch {

IGeneratorTracker::~IGeneratorTracker() {}

namespace Generators {

    GeneratorBase::~GeneratorBase() {}

    std::vector<size_t> randomiseIndices( size_t selectionSize, size_t sourceSize ) {

        assert( selectionSize <= sourceSize );
        std::vector<size_t> indices;
        indices.reserve( selectionSize );
        std::uniform_int_distribution<size_t> uid( 0, sourceSize-1 );

        std::set<size_t> seen;
        // !TBD: improve this algorithm
        while( indices.size() < selectionSize ) {
            auto index = uid( rng() );
            if( seen.insert( index ).second )
                indices.push_back( index );
        }
        return indices;
    }

    auto acquireGeneratorTracker( SourceLineInfo const& lineInfo ) -> IGeneratorTracker& {
        return getResultCapture().acquireGeneratorTracker( lineInfo );
    }

    template<>
    auto all<int>() -> Generator<int> {
        return range( std::numeric_limits<int>::min(), std::numeric_limits<int>::max() );
    }

} // namespace Generators
} // namespace Catch
