#include <catch2/matchers/catch_matchers_predicate.hpp>

std::string Catch::Matchers::Generic::Detail::finalizeDescription(const std::string& desc) {
    if (desc.empty()) {
        return "matches undescribed predicate";
    } else {
        return "matches predicate: \"" + desc + '"';
    }
}
