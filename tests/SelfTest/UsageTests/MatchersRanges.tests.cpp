// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_container_properties.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_quantifiers.hpp>
#include <catch2/matchers/catch_matchers_predicate.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <array>
#include <cmath>
#include <list>
#include <map>
#include <vector>

namespace {
namespace unrelated {
    class needs_ADL_begin {
        std::array<int, 5> elements{ {1, 2, 3, 4, 5} };
    public:
        using iterator = std::array<int, 5>::iterator;
        using const_iterator = std::array<int, 5>::const_iterator;

        const_iterator Begin() const { return elements.begin(); }
        const_iterator End() const { return elements.end(); }

        friend const_iterator begin(needs_ADL_begin const& lhs) {
            return lhs.Begin();
        }
        friend const_iterator end(needs_ADL_begin const& rhs) {
            return rhs.End();
        }
    };
} // end unrelated namespace

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-function"
#endif

class has_different_begin_end_types {
    std::array<int, 5> elements{ {1, 2, 3, 4, 5} };

    // Different type for the "end" iterator
    struct iterator_end {};
    // Just a fake forward iterator, that only compares to a different
    // type (so we can test two-type ranges).
    struct iterator {
        int const* start;
        int const* end;

        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = int;
        using reference = int const&;
        using pointer = int const*;


        friend bool operator==( iterator iter, iterator_end ) {
            return iter.start == iter.end;
        }
        friend bool operator!=( iterator iter, iterator_end ) {
            return iter.start != iter.end;
        }
        iterator& operator++() {
            ++start;
            return *this;
        }
        iterator operator++(int) {
            auto tmp(*this);
            ++start;
            return tmp;
        }
        reference operator*() const {
            return *start;
        }
        pointer operator->() const {
            return start;
        }
    };


public:
    iterator begin() const {
        return { elements.data(), elements.data() + elements.size() };
    }

    iterator_end end() const {
        return {};
    }
};

#if defined(__clang__)
#  pragma clang diagnostic pop
#endif


struct with_mocked_iterator_access {
    static constexpr size_t data_size = 5;
    std::array<int, data_size> elements{ {1, 2, 3, 4, 5} };
    std::array<bool, data_size> touched{};
    std::array<bool, data_size> derefed{};

    // We want to check which elements were touched when iterating, so
    // we can check whether iterator-using code traverses range correctly
    struct iterator {
        with_mocked_iterator_access* m_origin;
        size_t m_origin_idx;

        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = int;
        using reference = int const&;
        using pointer = int const*;

        friend bool operator==(iterator lhs, iterator rhs) {
            return lhs.m_origin == rhs.m_origin
                && lhs.m_origin_idx == rhs.m_origin_idx;
        }
        friend bool operator!=(iterator lhs, iterator rhs) {
            return !(lhs == rhs);
        }
        iterator& operator++() {
            ++m_origin_idx;
            assert(m_origin_idx < data_size + 1 && "Outside of valid alloc");
            if (m_origin_idx < data_size) {
                m_origin->touched[m_origin_idx] = true;
            }
            return *this;
        }
        iterator operator++(int) {
            auto tmp(*this);
            ++(*this);
            return tmp;
        }
        reference operator*() const {
            assert(m_origin_idx < data_size && "Attempted to deref invalid position");
            m_origin->derefed[m_origin_idx] = true;
            return m_origin->elements[m_origin_idx];
        }
        pointer operator->() const {
            assert(m_origin_idx < data_size && "Attempted to deref invalid position");
            return &m_origin->elements[m_origin_idx];
        }

    };

    iterator begin() const {
        // Const-cast away to avoid overcomplicating the iterators
        // We should actually fix this over time
        return { const_cast<with_mocked_iterator_access*>(this), 0 };
    }
    iterator end() const {
        return { const_cast<with_mocked_iterator_access*>(this), data_size };
    }

};

} // end anon namespace

namespace Catch {
    template <>
    struct StringMaker<with_mocked_iterator_access> {
        static std::string convert(with_mocked_iterator_access const& access) {
            // We have to avoid the type's iterators, because we check
            // their use in tests
            return ::Catch::Detail::stringify(access.elements);
        }
    };
}

struct MoveOnlyTestElement {
    int num = 0;
    MoveOnlyTestElement(int n) :num(n) {}

    MoveOnlyTestElement(MoveOnlyTestElement&& rhs) = default;
    MoveOnlyTestElement& operator=(MoveOnlyTestElement&& rhs) = default;

    friend bool operator==(MoveOnlyTestElement const& lhs, MoveOnlyTestElement const& rhs) {
        return lhs.num == rhs.num;
    }

    friend std::ostream& operator<<(std::ostream& out, MoveOnlyTestElement const& elem) {
        out << elem.num;
        return out;
    }
};

TEST_CASE("Basic use of the Contains range matcher", "[matchers][templated][contains]") {
    using Catch::Matchers::Contains;

    SECTION("Different argument ranges, same element type, default comparison") {
        std::array<int, 3> a{ { 1,2,3 } };
        std::vector<int> b{ 0,1,2 };
        std::list<int> c{ 4,5,6 };

        // A contains 1
        REQUIRE_THAT(a,  Contains(1));
        // B contains 1
        REQUIRE_THAT(b,  Contains(1));
        // C does not contain 1
        REQUIRE_THAT(c, !Contains(1));
    }

    SECTION("Different argument ranges, same element type, custom comparison") {
        std::array<int, 3> a{ { 1,2,3 } };
        std::vector<int> b{ 0,1,2 };
        std::list<int> c{ 4,5,6 };

        auto close_enough = [](int lhs, int rhs) { return std::abs(lhs - rhs) <= 1; };

        // A contains 1, which is "close enough" to 0
        REQUIRE_THAT(a,  Contains(0, close_enough));
        // B contains 0 directly
        REQUIRE_THAT(b,  Contains(0, close_enough));
        // C does not contain anything "close enough" to 0
        REQUIRE_THAT(c, !Contains(0, close_enough));
    }

    SECTION("Different element type, custom comparisons") {
        std::array<std::string, 3> a{ { "abc", "abcd" , "abcde" } };

        REQUIRE_THAT(a, Contains(4, [](auto&& lhs, size_t sz) {
            return lhs.size() == sz;
        }));
    }

    SECTION("Can handle type that requires ADL-found free function begin and end") {
        unrelated::needs_ADL_begin in;

        REQUIRE_THAT(in,  Contains(1));
        REQUIRE_THAT(in, !Contains(8));
    }

    SECTION("Initialization with move only types") {
        std::array<MoveOnlyTestElement, 3> in{ { MoveOnlyTestElement{ 1 }, MoveOnlyTestElement{ 2 }, MoveOnlyTestElement{ 3 } } };

        REQUIRE_THAT(in,  Contains(MoveOnlyTestElement{ 2 }));
        REQUIRE_THAT(in, !Contains(MoveOnlyTestElement{ 9 }));
    }

    SECTION("Matching using matcher") {
        std::array<double, 4> in{ {1, 2, 3} };

        REQUIRE_THAT(in, Contains(Catch::Matchers::WithinAbs(0.5, 0.5)));
    }
}

namespace {

    struct has_empty {
        bool empty() const { return false; }
    };

namespace unrelated {
    struct ADL_empty {
        bool Empty() const { return true; }

        friend bool empty(ADL_empty e) {
            return e.Empty();
        }
    };

} // end namespace unrelated
} // end unnamed namespace

TEST_CASE("Basic use of the Empty range matcher", "[matchers][templated][empty]") {
    using Catch::Matchers::IsEmpty;
    SECTION("Simple, std-provided containers") {
        std::array<int, 0> empty_array{};
        std::array<double, 1> non_empty_array{};
        REQUIRE_THAT(empty_array, IsEmpty());
        REQUIRE_THAT(non_empty_array, !IsEmpty());

        std::vector<std::string> empty_vec;
        std::vector<char> non_empty_vec{ 'a', 'b', 'c' };
        REQUIRE_THAT(empty_vec, IsEmpty());
        REQUIRE_THAT(non_empty_vec, !IsEmpty());

        std::list<std::list<std::list<int>>> inner_lists_are_empty;
        inner_lists_are_empty.push_back({});
        REQUIRE_THAT(inner_lists_are_empty, !IsEmpty());
        REQUIRE_THAT(inner_lists_are_empty.front(), IsEmpty());
    }
    SECTION("Type with empty") {
        REQUIRE_THAT(has_empty{}, !IsEmpty());
    }
    SECTION("Type requires ADL found empty free function") {
        REQUIRE_THAT(unrelated::ADL_empty{}, IsEmpty());
    }
}

namespace {
    class LessThanMatcher final : public Catch::Matchers::MatcherBase<size_t> {
        size_t m_target;
    public:
        explicit LessThanMatcher(size_t target):
            m_target(target)
        {}

        bool match(size_t const& size) const override {
            return size < m_target;
        }

        std::string describe() const override {
            return "is less than " + std::to_string(m_target);
        }
    };

    LessThanMatcher Lt(size_t sz) {
        return LessThanMatcher{ sz };
    }

    namespace unrelated {
        struct ADL_size {
            size_t sz() const {
                return 12;
            }
            friend size_t size(ADL_size s) {
                return s.sz();
            }
        };
    } // end namespace unrelated

    struct has_size {
        size_t size() const {
            return 13;
        }
    };

} // end unnamed namespace

TEST_CASE("Usage of the SizeIs range matcher", "[matchers][templated][size]") {
    using Catch::Matchers::SizeIs;
    SECTION("Some with stdlib containers") {
        std::vector<int> empty_vec;
        REQUIRE_THAT(empty_vec,  SizeIs(0));
        REQUIRE_THAT(empty_vec, !SizeIs(2));
        REQUIRE_THAT(empty_vec,  SizeIs(Lt(2)));

        std::array<int, 2> arr{};
        REQUIRE_THAT(arr,  SizeIs(2));
        REQUIRE_THAT(arr,  SizeIs( Lt(3)));
        REQUIRE_THAT(arr, !SizeIs(!Lt(3)));

        std::map<int, int> map{ {1, 1}, {2, 2}, {3, 3} };
        REQUIRE_THAT(map, SizeIs(3));
    }
    SECTION("Type requires ADL found size free function") {
        REQUIRE_THAT(unrelated::ADL_size{}, SizeIs(12));
    }
    SECTION("Type has size member") {
        REQUIRE_THAT(has_size{}, SizeIs(13));
    }
}


TEST_CASE("Usage of AllMatch range matcher", "[matchers][templated][quantifiers]") {
    using Catch::Matchers::AllMatch;
    using Catch::Matchers::Predicate;

    SECTION("Basic usage") {
        using Catch::Matchers::Contains;
        using Catch::Matchers::SizeIs;

        std::array<std::array<int, 5>, 5> data{{
                                        {{ 0, 1, 2, 3, 5 }},
                                        {{ 4,-3,-2, 5, 0 }},
                                        {{ 0, 0, 0, 5, 0 }},
                                        {{ 0,-5, 0, 5, 0 }},
                                        {{ 1, 0, 0,-1, 5 }}
        }};

        REQUIRE_THAT(data,  AllMatch(SizeIs(5)));
        REQUIRE_THAT(data, !AllMatch(Contains(0) && Contains(1)));
    }

    SECTION("Type requires ADL found begin and end") {
        unrelated::needs_ADL_begin needs_adl;
        REQUIRE_THAT( needs_adl, AllMatch( Predicate<int>( []( int elem ) {
                          return elem < 6;
                      } ) ) );
    }

    SECTION("Shortcircuiting") {
        with_mocked_iterator_access mocked;
        SECTION("All are read") {
            auto allMatch = AllMatch(Predicate<int>([](int elem) {
                return elem < 10;
            }));
            REQUIRE_THAT(mocked, allMatch);
            REQUIRE(mocked.derefed[0]);
            REQUIRE(mocked.derefed[1]);
            REQUIRE(mocked.derefed[2]);
            REQUIRE(mocked.derefed[3]);
            REQUIRE(mocked.derefed[4]);
        }
        SECTION("Short-circuited") {
            auto allMatch = AllMatch(Predicate<int>([](int elem) {
                return elem < 3;
            }));
            REQUIRE_THAT(mocked, !allMatch);
            REQUIRE(mocked.derefed[0]);
            REQUIRE(mocked.derefed[1]);
            REQUIRE(mocked.derefed[2]);
            REQUIRE_FALSE(mocked.derefed[3]);
            REQUIRE_FALSE(mocked.derefed[4]);
        }
    }
}

TEST_CASE("Usage of AnyMatch range matcher", "[matchers][templated][quantifiers]") {
    using Catch::Matchers::AnyMatch;
    using Catch::Matchers::Predicate;

    SECTION("Basic usage") {
        using Catch::Matchers::Contains;
        using Catch::Matchers::SizeIs;

        std::array<std::array<int, 5>, 5> data{ {
                                        {{ 0, 1, 2, 3, 5 }},
                                        {{ 4,-3,-2, 5, 0 }},
                                        {{ 0, 0, 0, 5, 0 }},
                                        {{ 0,-5, 0, 5, 0 }},
                                        {{ 1, 0, 0,-1, 5 }}
        } };

        REQUIRE_THAT(data,  AnyMatch(SizeIs(5)));
        REQUIRE_THAT(data, !AnyMatch(Contains(0) && Contains(10)));
    }

    SECTION("Type requires ADL found begin and end") {
        unrelated::needs_ADL_begin needs_adl;
        REQUIRE_THAT( needs_adl, AnyMatch( Predicate<int>( []( int elem ) {
                          return elem < 3;
                      } ) ) );
    }

    SECTION("Shortcircuiting") {
        with_mocked_iterator_access mocked;
        SECTION("All are read") {
            auto anyMatch = AnyMatch(
                Predicate<int>( []( int elem ) { return elem > 10; } ) );
            REQUIRE_THAT( mocked, !anyMatch );
            REQUIRE( mocked.derefed[0] );
            REQUIRE( mocked.derefed[1] );
            REQUIRE( mocked.derefed[2] );
            REQUIRE( mocked.derefed[3] );
            REQUIRE( mocked.derefed[4] );
        }
        SECTION("Short-circuited") {
            auto anyMatch = AnyMatch(
                Predicate<int>( []( int elem ) { return elem < 3; } ) );
            REQUIRE_THAT( mocked, anyMatch );
            REQUIRE( mocked.derefed[0] );
            REQUIRE_FALSE( mocked.derefed[1] );
            REQUIRE_FALSE( mocked.derefed[2] );
            REQUIRE_FALSE( mocked.derefed[3] );
            REQUIRE_FALSE( mocked.derefed[4] );
        }
    }
}

TEST_CASE("Usage of NoneMatch range matcher", "[matchers][templated][quantifiers]") {
    using Catch::Matchers::NoneMatch;
    using Catch::Matchers::Predicate;

    SECTION("Basic usage") {
        using Catch::Matchers::Contains;
        using Catch::Matchers::SizeIs;

        std::array<std::array<int, 5>, 5> data{ {
                                        {{ 0, 1, 2, 3, 5 }},
                                        {{ 4,-3,-2, 5, 0 }},
                                        {{ 0, 0, 0, 5, 0 }},
                                        {{ 0,-5, 0, 5, 0 }},
                                        {{ 1, 0, 0,-1, 5 }}
        } };

        REQUIRE_THAT(data, NoneMatch(SizeIs(6)));
        REQUIRE_THAT(data, !NoneMatch(Contains(0) && Contains(1)));
    }

    SECTION( "Type requires ADL found begin and end" ) {
        unrelated::needs_ADL_begin needs_adl;
        REQUIRE_THAT( needs_adl, NoneMatch( Predicate<int>( []( int elem ) {
                          return elem > 6;
                      } ) ) );
    }

    SECTION("Shortcircuiting") {
        with_mocked_iterator_access mocked;
        SECTION("All are read") {
            auto noneMatch = NoneMatch(
                Predicate<int>([](int elem) { return elem > 10; }));
            REQUIRE_THAT(mocked, noneMatch);
            REQUIRE(mocked.derefed[0]);
            REQUIRE(mocked.derefed[1]);
            REQUIRE(mocked.derefed[2]);
            REQUIRE(mocked.derefed[3]);
            REQUIRE(mocked.derefed[4]);
        }
        SECTION("Short-circuited") {
            auto noneMatch = NoneMatch(
                Predicate<int>([](int elem) { return elem < 3; }));
            REQUIRE_THAT(mocked, !noneMatch);
            REQUIRE(mocked.derefed[0]);
            REQUIRE_FALSE(mocked.derefed[1]);
            REQUIRE_FALSE(mocked.derefed[2]);
            REQUIRE_FALSE(mocked.derefed[3]);
            REQUIRE_FALSE(mocked.derefed[4]);
        }
    }
}


// This is a C++17 extension, and GCC refuses to compile such code
// unless it is set to C++17 or later
#if defined(CATCH_CPP17_OR_GREATER)

TEST_CASE( "The quantifier range matchers support types with different types returned from begin and end",
           "[matchers][templated][quantifiers][approvals]" ) {
    using Catch::Matchers::AllMatch;
    using Catch::Matchers::AnyMatch;
    using Catch::Matchers::NoneMatch;

    using Catch::Matchers::Predicate;

    has_different_begin_end_types diff_types;
    REQUIRE_THAT( diff_types, !AllMatch( Predicate<int>( []( int elem ) {
                      return elem < 3;
                  } ) ) );

    REQUIRE_THAT( diff_types, AnyMatch( Predicate<int>( []( int elem ) {
                      return elem < 2;
                  } ) ) );

    REQUIRE_THAT( diff_types, !NoneMatch( Predicate<int>( []( int elem ) {
                      return elem < 3;
                  } ) ) );
}

#endif
