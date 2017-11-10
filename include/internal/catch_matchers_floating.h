/*
 *  Created by Martin on 07/11/2017.
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_MATCHERS_FLOATING_H_INCLUDED
#define TWOBLUECUBES_CATCH_MATCHERS_FLOATING_H_INCLUDED

#include "catch_matchers.h"

#include <type_traits>
#include <cmath>

namespace Catch {
namespace Matchers {

    namespace Floating {

        enum class FloatingPointKind : uint8_t;

        struct WithinAbsMatcher : MatcherBase<double> {
            WithinAbsMatcher(double target, double margin);
            bool match(double const& matchee) const override;
            std::string describe() const override;
        private:
            double m_target;
            double m_margin;
        };

        struct WithinUlpsMatcher : MatcherBase<double> {
            WithinUlpsMatcher(double target, int ulps, FloatingPointKind baseType);
            bool match(double const& matchee) const override;
            std::string describe() const override;
        private:
            double m_target;
            int m_ulps;
            FloatingPointKind m_type;
        };


    } // namespace Floating

    // The following functions create the actual matcher objects.
    // This allows the types to be inferred
    Floating::WithinUlpsMatcher WithinULP(double target, int maxUlpDiff);
    Floating::WithinUlpsMatcher WithinULP(float target, int maxUlpDiff);
    Floating::WithinAbsMatcher WithinAbs(double target, double margin);

} // namespace Matchers
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_MATCHERS_FLOATING_H_INCLUDED
