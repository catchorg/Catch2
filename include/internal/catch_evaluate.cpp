/*
 *  Created by Martin on 01/08/2017.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_evaluate.hpp"

#include "catch_enforce.h"

namespace Catch {
namespace Internal {

    const char* operatorName(Operator op) {
        switch (op) {
            case IsEqualTo:
                return "==";
            case IsNotEqualTo:
                return "!=";
            case IsLessThan:
                return "<";
            case IsGreaterThan:
                return ">";
            case IsLessThanOrEqualTo:
                return "<=";
            case IsGreaterThanOrEqualTo:
                return ">=";
            default:
                CATCH_ERROR("Attempting to translate unknown operator!");
        }
    }

    // nullptr_t support based on pull request #154 from Konstantin Baumann
    std::nullptr_t opCast(std::nullptr_t) { return nullptr; }

} // end of namespace Internal
} // end of namespace Catch
