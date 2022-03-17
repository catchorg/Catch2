#include <catch2/internal/catch_test_case_info_hasher.hpp>

#include <iostream>

namespace Catch {
    TestCaseInfoHasher::hash_t TestCaseInfoHasher::operator()( TestCaseInfo const& t ) const {
        // FNV-1a algorithm:
        const hash_t prime = 1099511628211u;
        hash_t hash = 14695981039346656037u;
        for (const char c : t.name) {
            hash ^= c;
            hash *= prime;
        }
        for (const char c : t.className) {
            hash ^= c;
            hash *= prime;
        }
        // first option to hash the tag(s):
        for (const Tag& tag : t.tags)
        {
            for (const char c : tag.original)
            {
                hash ^= c;
                hash *= prime;
            }
        }
        return hash;
    }
} // namespace Catch
