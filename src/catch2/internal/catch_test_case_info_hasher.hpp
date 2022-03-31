#ifndef CATCH_TEST_CASE_INFO_HASHER_HPP_INCLUDED
#define CATCH_TEST_CASE_INFO_HASHER_HPP_INCLUDED

#include <cstdint>

namespace Catch {

    struct TestCaseInfo;

    class TestCaseInfoHasher {
    public:
        using hash_t = std::uint64_t;
        TestCaseInfoHasher( hash_t seed );
        uint32_t operator()( TestCaseInfo const& t ) const;

    private:
        hash_t m_seed;
    };

} // namespace Catch

#endif /* CATCH_TEST_CASE_INFO_HASHER_HPP_INCLUDED */
