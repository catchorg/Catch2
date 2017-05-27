/*
 *  Created by Martin on 27/5/2017.
 *  Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <iostream>

struct truthy {
    truthy(bool b):m_value(b){}
    operator bool() const {
        return false;
    }
    bool m_value;
};

std::ostream& operator<<(std::ostream& o, truthy) {
    o << "Hey, its truthy!";
    return o;
}

#include "catch.hpp"

TEST_CASE( "Reconstruction should be based on stringification: #914" , "[Decomposition][failing][.]") {
    CHECK(truthy(false));
}
