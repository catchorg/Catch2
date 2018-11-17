/*
 *  Created by Martin on 07/11/2017.
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_matchers_floating.h"
#include "catch_enforce.h"
#include "catch_polyfills.hpp"
#include "catch_to_string.hpp"
#include "catch_tostring.h"

#include <cstdlib>
#include <cstdint>
#include <cstring>

namespace Catch {
namespace Matchers {
namespace Floating {
enum class FloatingPointKind : uint8_t {
    Float,
    Double
};
}
}
}

namespace {

template <typename T>
struct Converter;

template <>
struct Converter<float> {
    static_assert(sizeof(float) == sizeof(int32_t), "Important ULP matcher assumption violated");
    Converter(float f) {
        std::memcpy(&i, &f, sizeof(f));
    }
    int32_t i;
};

template <>
struct Converter<double> {
    static_assert(sizeof(double) == sizeof(int64_t), "Important ULP matcher assumption violated");
    Converter(double d) {
        std::memcpy(&i, &d, sizeof(d));
    }
    int64_t i;
};

template <typename T>
auto convert(T t) -> Converter<T> {
    return Converter<T>(t);
}

template <typename FP>
bool almostEqualUlps(FP lhs, FP rhs, int maxUlpDiff) {
    // Comparison with NaN should always be false.
    // This way we can rule it out before getting into the ugly details
    if (Catch::isnan(lhs) || Catch::isnan(rhs)) {
        return false;
    }

    auto lc = convert(lhs);
    auto rc = convert(rhs);

    if ((lc.i < 0) != (rc.i < 0)) {
        // Potentially we can have +0 and -0
        return lhs == rhs;
    }

    auto ulpDiff = std::abs(lc.i - rc.i);
    return ulpDiff <= maxUlpDiff;
}

}


namespace Catch {
namespace Matchers {
namespace Floating {
    WithinAbsMatcher::WithinAbsMatcher(double target, double margin)
        :m_target{ target }, m_margin{ margin } {
        CATCH_ENFORCE(margin >= 0, "Invalid margin: " << margin << '.'
            << " Margin has to be non-negative.");
    }

    // Performs equivalent check of std::fabs(lhs - rhs) <= margin
    // But without the subtraction to allow for INFINITY in comparison
    bool WithinAbsMatcher::match(double const& matchee) const {
        return (matchee + m_margin >= m_target) && (m_target + m_margin >= matchee);
    }

    std::string WithinAbsMatcher::describe() const {
        return "is within " + ::Catch::Detail::stringify(m_margin) + " of " + ::Catch::Detail::stringify(m_target);
    }


    WithinUlpsMatcher::WithinUlpsMatcher(double target, int ulps, FloatingPointKind baseType)
        :m_target{ target }, m_ulps{ ulps }, m_type{ baseType } {
        CATCH_ENFORCE(ulps >= 0, "Invalid ULP setting: " << ulps << '.'
            << " ULPs have to be non-negative.");
    }

#if defined(__clang__)
#pragma clang diagnostic push
// Clang <3.5 reports on the default branch in the switch below
#pragma clang diagnostic ignored "-Wunreachable-code"
#endif

    bool WithinUlpsMatcher::match(double const& matchee) const {
        switch (m_type) {
        case FloatingPointKind::Float:
            return almostEqualUlps<float>(static_cast<float>(matchee), static_cast<float>(m_target), m_ulps);
        case FloatingPointKind::Double:
            return almostEqualUlps<double>(matchee, m_target, m_ulps);
        default:
            CATCH_INTERNAL_ERROR( "Unknown FloatingPointKind value" );
        }
    }

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

    std::string WithinUlpsMatcher::describe() const {
        return "is within " + Catch::to_string(m_ulps) + " ULPs of " + ::Catch::Detail::stringify(m_target) + ((m_type == FloatingPointKind::Float)? "f" : "");
    }

}// namespace Floating



Floating::WithinUlpsMatcher WithinULP(double target, int maxUlpDiff) {
    return Floating::WithinUlpsMatcher(target, maxUlpDiff, Floating::FloatingPointKind::Double);
}

Floating::WithinUlpsMatcher WithinULP(float target, int maxUlpDiff) {
    return Floating::WithinUlpsMatcher(target, maxUlpDiff, Floating::FloatingPointKind::Float);
}

Floating::WithinAbsMatcher WithinAbs(double target, double margin) {
    return Floating::WithinAbsMatcher(target, margin);
}

} // namespace Matchers
} // namespace Catch

