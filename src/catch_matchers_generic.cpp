#include "catch_matchers_generic.hpp"

std::string Catch::Matchers::Generic::Detail::finalizeDescription(const std::string& desc) {
    if (desc.empty()) {
        return "matches undescribed predicate";
    } else {
        return "matches predicate: \"" + desc + '"';
    }
}
