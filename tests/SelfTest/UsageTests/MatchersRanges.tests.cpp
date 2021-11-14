
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_container_properties.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_quantifiers.hpp>
#include <catch2/matchers/catch_matchers_predicate.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <cmath>
#include <initializer_list>
#include <list>
#include <map>
#include <type_traits>
#include <vector>
#include <memory>

namespace {

namespace unrelated {
    template <typename T>
    class needs_ADL_begin {
        std::vector<T> m_elements;
    public:
        using iterator = typename std::vector<T>::iterator;
        using const_iterator = typename std::vector<T>::const_iterator;

        needs_ADL_begin(std::initializer_list<T> init) : m_elements(init) {}

        const_iterator Begin() const { return m_elements.begin(); }
        const_iterator End() const { return m_elements.end(); }

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

template <typename T>
class has_different_begin_end_types {
    // Using std::vector<T> leads to annoying issues when T is bool
    // so we just use list because the perf is not critical and ugh.
    std::list<T> m_elements;

    // Different type for the "end" iterator
    struct iterator_end {};
    // Fake-ish forward iterator that only compares to a different type
    class iterator {
        using underlying_iter = typename std::list<T>::const_iterator;
        underlying_iter m_start;
        underlying_iter m_end;

    public:
        iterator( underlying_iter start, underlying_iter end ):
            m_start( start ), m_end( end ) {}

        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using const_reference = T const&;
        using pointer = T const*;


        friend bool operator==( iterator iter, iterator_end ) {
            return iter.m_start == iter.m_end;
        }
        friend bool operator!=( iterator iter, iterator_end ) {
            return iter.m_start != iter.m_end;
        }
        iterator& operator++() {
            ++m_start;
            return *this;
        }
        iterator operator++(int) {
            auto tmp(*this);
            ++m_start;
            return tmp;
        }
        const_reference operator*() const {
            return *m_start;
        }
        pointer operator->() const {
            return m_start;
        }
    };


public:
    explicit has_different_begin_end_types( std::initializer_list<T> init ):
        m_elements( init ) {}

    iterator begin() const {
        return { m_elements.begin(), m_elements.end() };
    }

    iterator_end end() const {
        return {};
    }
};

#if defined(__clang__)
#  pragma clang diagnostic pop
#endif

template <typename T> struct with_mocked_iterator_access {
    std::vector<T> m_elements;

    // use plain arrays to have nicer printouts with CHECK(...)
    mutable std::unique_ptr<bool[]> m_derefed;

    // We want to check which elements were dereferenced when iterating, so
    // we can check whether iterator-using code traverses range correctly
    template <bool is_const> class basic_iterator {
        template <typename U>
        using constify_t = std::conditional_t<is_const, std::add_const_t<U>, U>;

        constify_t<with_mocked_iterator_access>* m_origin;
        size_t m_origin_idx;

    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = constify_t<T>;
        using const_reference = typename std::vector<T>::const_reference;
        using reference = typename std::vector<T>::reference;
        using pointer = typename std::vector<T>::pointer;

        basic_iterator( constify_t<with_mocked_iterator_access>* origin,
                        std::size_t origin_idx ):
            m_origin{ origin }, m_origin_idx{ origin_idx } {}

        friend bool operator==( basic_iterator lhs, basic_iterator rhs ) {
            return lhs.m_origin == rhs.m_origin &&
                   lhs.m_origin_idx == rhs.m_origin_idx;
        }
        friend bool operator!=( basic_iterator lhs, basic_iterator rhs ) {
            return !( lhs == rhs );
        }
        basic_iterator& operator++() {
            ++m_origin_idx;
            return *this;
        }
        basic_iterator operator++( int ) {
            auto tmp( *this );
            ++( *this );
            return tmp;
        }
        const_reference operator*() const {
            assert( m_origin_idx < m_origin->m_elements.size() && "Attempted to deref invalid position" );
            m_origin->m_derefed[m_origin_idx] = true;
            return m_origin->m_elements[m_origin_idx];
        }
        pointer operator->() const {
            assert( m_origin_idx < m_origin->m_elements.size() && "Attempted to deref invalid position" );
            return &m_origin->m_elements[m_origin_idx];
        }
    };

    using iterator = basic_iterator<false>;
    using const_iterator = basic_iterator<true>;

    with_mocked_iterator_access( std::initializer_list<T> init ):
        m_elements( init ),
        m_derefed( std::make_unique<bool[]>( m_elements.size() ) ) {}

    const_iterator begin() const { return { this, 0 }; }
    const_iterator end() const { return { this, m_elements.size() }; }
    iterator begin() { return { this, 0 }; }
    iterator end() { return { this, m_elements.size() }; }
};

} // end anon namespace

namespace Catch {
    // make sure with_mocked_iterator_access is not considered a range by Catch,
    // so that below StringMaker is used instead of the default one for ranges
    template <typename T>
    struct is_range<with_mocked_iterator_access<T>> : std::false_type {};

    template <typename T>
    struct StringMaker<with_mocked_iterator_access<T>> {
        static std::string
        convert( with_mocked_iterator_access<T> const& access ) {
            // We have to avoid the type's iterators, because we check
            // their use in tests
            return ::Catch::Detail::stringify( access.m_elements );
        }
    };
} // namespace Catch

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
        unrelated::needs_ADL_begin<int> in{1, 2, 3, 4, 5};

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
        unrelated::needs_ADL_begin<int> needs_adl{ 1, 2, 3, 4, 5 };
        REQUIRE_THAT( needs_adl, AllMatch( Predicate<int>( []( int elem ) {
                          return elem < 6;
                      } ) ) );
    }

    SECTION("Shortcircuiting") {
        with_mocked_iterator_access<int> mocked{ 1, 2, 3, 4, 5 };
        SECTION("All are read") {
            auto allMatch = AllMatch(Predicate<int>([](int elem) {
                return elem < 10;
            }));
            REQUIRE_THAT(mocked, allMatch);
            REQUIRE(mocked.m_derefed[0]);
            REQUIRE(mocked.m_derefed[1]);
            REQUIRE(mocked.m_derefed[2]);
            REQUIRE(mocked.m_derefed[3]);
            REQUIRE(mocked.m_derefed[4]);
        }
        SECTION("Short-circuited") {
            auto allMatch = AllMatch(Predicate<int>([](int elem) {
                return elem < 3;
            }));
            REQUIRE_THAT(mocked, !allMatch);
            REQUIRE(mocked.m_derefed[0]);
            REQUIRE(mocked.m_derefed[1]);
            REQUIRE(mocked.m_derefed[2]);
            REQUIRE_FALSE(mocked.m_derefed[3]);
            REQUIRE_FALSE(mocked.m_derefed[4]);
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

    SECTION( "Type requires ADL found begin and end" ) {
        unrelated::needs_ADL_begin<int> needs_adl{ 1, 2, 3, 4, 5 };
        REQUIRE_THAT( needs_adl, AnyMatch( Predicate<int>( []( int elem ) {
                          return elem < 3;
                      } ) ) );
    }

    SECTION("Shortcircuiting") {
        with_mocked_iterator_access<int> mocked{ 1, 2, 3, 4, 5 };
        SECTION("All are read") {
            auto anyMatch = AnyMatch(
                Predicate<int>( []( int elem ) { return elem > 10; } ) );
            REQUIRE_THAT( mocked, !anyMatch );
            REQUIRE( mocked.m_derefed[0] );
            REQUIRE( mocked.m_derefed[1] );
            REQUIRE( mocked.m_derefed[2] );
            REQUIRE( mocked.m_derefed[3] );
            REQUIRE( mocked.m_derefed[4] );
        }
        SECTION("Short-circuited") {
            auto anyMatch = AnyMatch(
                Predicate<int>( []( int elem ) { return elem < 3; } ) );
            REQUIRE_THAT( mocked, anyMatch );
            REQUIRE( mocked.m_derefed[0] );
            REQUIRE_FALSE( mocked.m_derefed[1] );
            REQUIRE_FALSE( mocked.m_derefed[2] );
            REQUIRE_FALSE( mocked.m_derefed[3] );
            REQUIRE_FALSE( mocked.m_derefed[4] );
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
        unrelated::needs_ADL_begin<int> needs_adl{ 1, 2, 3, 4, 5 };
        REQUIRE_THAT( needs_adl, NoneMatch( Predicate<int>( []( int elem ) {
                          return elem > 6;
                      } ) ) );
    }

    SECTION("Shortcircuiting") {
        with_mocked_iterator_access<int> mocked{ 1, 2, 3, 4, 5 };
        SECTION("All are read") {
            auto noneMatch = NoneMatch(
                Predicate<int>([](int elem) { return elem > 10; }));
            REQUIRE_THAT(mocked, noneMatch);
            REQUIRE(mocked.m_derefed[0]);
            REQUIRE(mocked.m_derefed[1]);
            REQUIRE(mocked.m_derefed[2]);
            REQUIRE(mocked.m_derefed[3]);
            REQUIRE(mocked.m_derefed[4]);
        }
        SECTION("Short-circuited") {
            auto noneMatch = NoneMatch(
                Predicate<int>([](int elem) { return elem < 3; }));
            REQUIRE_THAT(mocked, !noneMatch);
            REQUIRE(mocked.m_derefed[0]);
            REQUIRE_FALSE(mocked.m_derefed[1]);
            REQUIRE_FALSE(mocked.m_derefed[2]);
            REQUIRE_FALSE(mocked.m_derefed[3]);
            REQUIRE_FALSE(mocked.m_derefed[4]);
        }
    }
}

namespace {
    struct ConvertibleToBool
    {
        bool v;

        explicit operator bool() const
        {
            return v;
        }
    };
}

namespace Catch {
    template <>
    struct StringMaker<ConvertibleToBool> {
        static std::string
        convert( ConvertibleToBool const& convertible_to_bool ) {
            return ::Catch::Detail::stringify( convertible_to_bool.v );
        }
    };
} // namespace Catch

TEST_CASE("Usage of AllTrue range matcher", "[matchers][templated][quantifiers]") {
    using Catch::Matchers::AllTrue;

    SECTION( "Basic usage" ) {
        SECTION( "All true evaluates to true" ) {
            std::array<bool, 5> const data{ { true, true, true, true, true } };
            REQUIRE_THAT( data, AllTrue() );
        }
        SECTION( "Empty evaluates to true" ) {
            std::array<bool, 0> const data{};
            REQUIRE_THAT( data, AllTrue() );
        }
        SECTION( "One false evalutes to false" ) {
            std::array<bool, 5> const data{ { true, true, false, true, true } };
            REQUIRE_THAT( data, !AllTrue() );
        }
        SECTION( "All false evaluates to false" ) {
            std::array<bool, 5> const data{
                { false, false, false, false, false } };
            REQUIRE_THAT( data, !AllTrue() );
        }
    }

    SECTION( "Contained type is convertible to bool" ) {
        SECTION( "All true evaluates to true" ) {
            std::array<ConvertibleToBool, 5> const data{
                { { true }, { true }, { true }, { true }, { true } } };
            REQUIRE_THAT( data, AllTrue() );
        }
        SECTION( "One false evalutes to false" ) {
            std::array<ConvertibleToBool, 5> const data{
                { { true }, { true }, { false }, { true }, { true } } };
            REQUIRE_THAT( data, !AllTrue() );
        }
        SECTION( "All false evaluates to false" ) {
            std::array<ConvertibleToBool, 5> const data{
                { { false }, { false }, { false }, { false }, { false } } };
            REQUIRE_THAT( data, !AllTrue() );
        }
    }

    SECTION( "Shortcircuiting" ) {
        SECTION( "All are read" ) {
            with_mocked_iterator_access<bool> const mocked{
                true, true, true, true, true };
            REQUIRE_THAT( mocked, AllTrue() );
            REQUIRE( mocked.m_derefed[0] );
            REQUIRE( mocked.m_derefed[1] );
            REQUIRE( mocked.m_derefed[2] );
            REQUIRE( mocked.m_derefed[3] );
            REQUIRE( mocked.m_derefed[4] );
        }
        SECTION( "Short-circuited" ) {
            with_mocked_iterator_access<bool> const mocked{
                true, true, false, true, true };
            REQUIRE_THAT( mocked, !AllTrue() );
            REQUIRE( mocked.m_derefed[0] );
            REQUIRE( mocked.m_derefed[1] );
            REQUIRE( mocked.m_derefed[2] );
            REQUIRE_FALSE( mocked.m_derefed[3] );
            REQUIRE_FALSE( mocked.m_derefed[4] );
        }
    }
}

TEST_CASE( "Usage of NoneTrue range matcher", "[matchers][templated][quantifiers]" ) {
    using Catch::Matchers::NoneTrue;

    SECTION( "Basic usage" ) {
        SECTION( "All true evaluates to false" ) {
            std::array<bool, 5> const data{ { true, true, true, true, true } };
            REQUIRE_THAT( data, !NoneTrue() );
        }
        SECTION( "Empty evaluates to true" ) {
            std::array<bool, 0> const data{};
            REQUIRE_THAT( data, NoneTrue() );
        }
        SECTION( "One true evalutes to false" ) {
            std::array<bool, 5> const data{
                { false, false, true, false, false } };
            REQUIRE_THAT( data, !NoneTrue() );
        }
        SECTION( "All false evaluates to true" ) {
            std::array<bool, 5> const data{
                { false, false, false, false, false } };
            REQUIRE_THAT( data, NoneTrue() );
        }
    }

    SECTION( "Contained type is convertible to bool" ) {
        SECTION( "All true evaluates to false" ) {
            std::array<ConvertibleToBool, 5> const data{
                { { true }, { true }, { true }, { true }, { true } } };
            REQUIRE_THAT( data, !NoneTrue() );
        }
        SECTION( "One true evalutes to false" ) {
            std::array<ConvertibleToBool, 5> const data{
                { { false }, { false }, { true }, { false }, { false } } };
            REQUIRE_THAT( data, !NoneTrue() );
        }
        SECTION( "All false evaluates to true" ) {
            std::array<ConvertibleToBool, 5> const data{
                { { false }, { false }, { false }, { false }, { false } } };
            REQUIRE_THAT( data, NoneTrue() );
        }
    }

    SECTION( "Shortcircuiting" ) {
        SECTION( "All are read" ) {
            with_mocked_iterator_access<bool> const mocked{
                false, false, false, false, false };
            REQUIRE_THAT( mocked, NoneTrue() );
            REQUIRE( mocked.m_derefed[0] );
            REQUIRE( mocked.m_derefed[1] );
            REQUIRE( mocked.m_derefed[2] );
            REQUIRE( mocked.m_derefed[3] );
            REQUIRE( mocked.m_derefed[4] );
        }
        SECTION( "Short-circuited" ) {
            with_mocked_iterator_access<bool> const mocked{
                false, false, true, true, true };
            REQUIRE_THAT( mocked, !NoneTrue() );
            REQUIRE( mocked.m_derefed[0] );
            REQUIRE( mocked.m_derefed[1] );
            REQUIRE( mocked.m_derefed[2] );
            REQUIRE_FALSE( mocked.m_derefed[3] );
            REQUIRE_FALSE( mocked.m_derefed[4] );
        }
    }
}

TEST_CASE( "Usage of AnyTrue range matcher", "[matchers][templated][quantifiers]" ) {
    using Catch::Matchers::AnyTrue;

    SECTION( "Basic usage" ) {
        SECTION( "All true evaluates to true" ) {
            std::array<bool, 5> const data{ { true, true, true, true, true } };
            REQUIRE_THAT( data, AnyTrue() );
        }
        SECTION( "Empty evaluates to false" ) {
            std::array<bool, 0> const data{};
            REQUIRE_THAT( data, !AnyTrue() );
        }
        SECTION( "One true evalutes to true" ) {
            std::array<bool, 5> const data{
                { false, false, true, false, false } };
            REQUIRE_THAT( data, AnyTrue() );
        }
        SECTION( "All false evaluates to false" ) {
            std::array<bool, 5> const data{
                { false, false, false, false, false } };
            REQUIRE_THAT( data, !AnyTrue() );
        }
    }

    SECTION( "Contained type is convertible to bool" ) {
        SECTION( "All true evaluates to true" ) {
            std::array<ConvertibleToBool, 5> const data{
                { { true }, { true }, { true }, { true }, { true } } };
            REQUIRE_THAT( data, AnyTrue() );
        }
        SECTION( "One true evalutes to true" ) {
            std::array<ConvertibleToBool, 5> const data{
                { { false }, { false }, { true }, { false }, { false } } };
            REQUIRE_THAT( data, AnyTrue() );
        }
        SECTION( "All false evaluates to false" ) {
            std::array<ConvertibleToBool, 5> const data{
                { { false }, { false }, { false }, { false }, { false } } };
            REQUIRE_THAT( data, !AnyTrue() );
        }
    }

    SECTION( "Shortcircuiting" ) {
        SECTION( "All are read" ) {
            with_mocked_iterator_access<bool> const mocked{
                false, false, false, false, true };
            REQUIRE_THAT( mocked, AnyTrue() );
            REQUIRE( mocked.m_derefed[0] );
            REQUIRE( mocked.m_derefed[1] );
            REQUIRE( mocked.m_derefed[2] );
            REQUIRE( mocked.m_derefed[3] );
            REQUIRE( mocked.m_derefed[4] );
        }
        SECTION( "Short-circuited" ) {
            with_mocked_iterator_access<bool> const mocked{
                false, false, true, true, true };
            REQUIRE_THAT( mocked, AnyTrue() );
            REQUIRE( mocked.m_derefed[0] );
            REQUIRE( mocked.m_derefed[1] );
            REQUIRE( mocked.m_derefed[2] );
            REQUIRE_FALSE( mocked.m_derefed[3] );
            REQUIRE_FALSE( mocked.m_derefed[4] );
        }
    }
}

TEST_CASE("All/Any/None True matchers support types with ADL begin",
          "[approvals][matchers][quantifiers][templated]") {
    using Catch::Matchers::AllTrue;
    using Catch::Matchers::NoneTrue;
    using Catch::Matchers::AnyTrue;


    SECTION( "Type requires ADL found begin and end" ) {
        unrelated::needs_ADL_begin<bool> const needs_adl{
            true, true, true, true, true };
        REQUIRE_THAT( needs_adl, AllTrue() );
    }

    SECTION( "Type requires ADL found begin and end" ) {
        unrelated::needs_ADL_begin<bool> const needs_adl{
            false, false, false, false, false };
        REQUIRE_THAT( needs_adl, NoneTrue() );
    }

    SECTION( "Type requires ADL found begin and end" ) {
        unrelated::needs_ADL_begin<bool> const needs_adl{
            false, false, true, false, false };
        REQUIRE_THAT( needs_adl, AnyTrue() );
    }
}

// Range loop iterating over range with different types for begin and end is a
// C++17 feature, and GCC refuses to compile such code unless the lang mode is
// set to C++17 or later.
#if defined(CATCH_CPP17_OR_GREATER)

TEST_CASE( "The quantifier range matchers support types with different types returned from begin and end",
           "[matchers][templated][quantifiers][approvals]" ) {
    using Catch::Matchers::AllMatch;
    using Catch::Matchers::AllTrue;
    using Catch::Matchers::AnyMatch;
    using Catch::Matchers::AnyTrue;
    using Catch::Matchers::NoneMatch;
    using Catch::Matchers::NoneTrue;

    using Catch::Matchers::Predicate;

    SECTION( "AllAnyNoneMatch" ) {
        has_different_begin_end_types<int> diff_types{ 1, 2, 3, 4, 5 };
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
    SECTION( "AllAnyNoneTrue" ) {
        has_different_begin_end_types<bool> diff_types{ false, false, true, false, false };

        REQUIRE_THAT( diff_types, !AllTrue() );
        REQUIRE_THAT( diff_types, AnyTrue() );
        REQUIRE_THAT( diff_types, !NoneTrue() );
    }
}

#endif
