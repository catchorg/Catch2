#include <catch2/catch_test_case_info.hpp>
#include <catch2/internal/catch_test_case_info_hasher.hpp>

namespace Catch {
    TestCaseInfoHasher::TestCaseInfoHasher( hash_t seed ): m_seed( seed ) {}

    TestCaseInfoHasher::TestCaseInfoHasher(): m_seed( 14695981039346656037u ) {}

    TestCaseInfoHasher::hash_t
    TestCaseInfoHasher::operator()( TestCaseInfo const& t ) const {
        // FNV-1a hash algorithm that is designed for uniqueness:
        const hash_t prime = 1099511628211u;
        hash_t hash = m_seed;
        for ( const char c : t.name ) {
            hash ^= c;
            hash *= prime;
        }
        for ( const char c : t.className ) {
            hash ^= c;
            hash *= prime;
        }
        for ( const Tag& tag : t.tags ) {
            for ( const char c : tag.original ) {
                hash ^= c;
                hash *= prime;
            }
        }
        return hash;
    }
} // namespace Catch
