#ifndef CATCH_TEST_CASE_INFO_HASHER_HPP_INCLUDED
#define CATCH_TEST_CASE_INFO_HASHER_HPP_INCLUDED

#include <catch2/catch_test_case_info.hpp>

#include <cstdint>

namespace Catch {

class TestCaseInfoHasher {
public:
    using hash_t = std::uint64_t;
    TestCaseInfoHasher() = default;
    ~TestCaseInfoHasher() = default;
    hash_t operator()( TestCaseInfo const& t ) const;
};

}

#endif /* CATCH_TEST_CASE_INFO_HASHER_HPP_INCLUDED */
