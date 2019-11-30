/*
 *  Created by Phil Nash on 8/8/2017.
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_decomposer.h"
#include "catch_config.hpp"

namespace Catch {

    ITransientExpression::~ITransientExpression() = default;
    
    void formatReconstructedExpression( std::ostream &os, std::string const& lhs, StringRef op, std::string const& rhs ) {
        if( lhs.size() + rhs.size() < 40 &&
                lhs.find('\n') == std::string::npos &&
                rhs.find('\n') == std::string::npos )
            os << lhs << " " << op << " " << rhs;
        else
            os << lhs << "\n" << op << "\n" << rhs;
    }
}
